#include "EllipsoidTerrainLoader.h"

#include <Cesium3DTilesContent/QuantizedMeshLoader.h>
#include <Cesium3DTilesContent/upsampleGltfForRasterOverlays.h>
#include <CesiumAsync/IAssetResponse.h>
#include <CesiumGeospatial/calcQuadtreeMaxGeometricError.h>
#include <CesiumGltfContent/GltfUtilities.h>
#include <CesiumRasterOverlays/RasterOverlayUtilities.h>
#include <CesiumUtility/JsonHelpers.h>
#include <CesiumUtility/Uri.h>

#include <libmorton/morton.h>
#include <rapidjson/document.h>

using namespace CesiumAsync;
using namespace Cesium3DTilesContent;
using namespace Cesium3DTilesSelection;
using namespace CesiumGeometry;
using namespace CesiumGeospatial;
using namespace CesiumRasterOverlays;
using namespace CesiumUtility;

namespace {
BoundingVolume createDefaultLooseEarthBoundingVolume(
    const CesiumGeospatial::GlobeRectangle& globeRectangle) {
  return BoundingRegionWithLooseFittingHeights(
      BoundingRegion(globeRectangle, 0, 0));
}

TileLoadResult convertToTileLoadResult(QuantizedMeshLoadResult&& loadResult) {
  if (loadResult.errors || !loadResult.model) {
    return TileLoadResult::createFailedResult(loadResult.pRequest);
  }

  return TileLoadResult{
      std::move(*loadResult.model),
      CesiumGeometry::Axis::Y,
      loadResult.updatedBoundingVolume,
      std::nullopt,
      std::nullopt,
      nullptr,
      {},
      TileLoadResultState::Success};
}
} // namespace

namespace Cesium3DTilesSelection {

bool EllipsoidTerrainLoader::isProjectionString(const std::string& str) {
  return str == EPSG4326 || str == EPSG3857;
}

CesiumAsync::Future<TilesetContentLoaderResult<EllipsoidTerrainLoader>>
EllipsoidTerrainLoader::createLoader(
    const TilesetExternals& externals,
    const std::string& projectionString) {

  CesiumGeospatial::Projection projection;
  CesiumGeospatial::GlobeRectangle quadtreeRectangleGlobe(0.0, 0.0, 0.0, 0.0);
  CesiumGeometry::Rectangle quadtreeRectangleProjected(0.0, 0.0, 0.0, 0.0);
  uint32_t quadtreeXTiles;

  if (projectionString == EPSG4326) {
    CesiumGeospatial::GeographicProjection geographic;
    projection = geographic;
    quadtreeRectangleGlobe = GeographicProjection::MAXIMUM_GLOBE_RECTANGLE;
    quadtreeRectangleProjected = geographic.project(quadtreeRectangleGlobe);
    quadtreeXTiles = 2;
  } else if (projectionString == EPSG3857) {
    CesiumGeospatial::WebMercatorProjection webMercator;
    projection = webMercator;
    quadtreeRectangleGlobe = WebMercatorProjection::MAXIMUM_GLOBE_RECTANGLE;
    quadtreeRectangleProjected = webMercator.project(quadtreeRectangleGlobe);
    quadtreeXTiles = 1;
  } else {
    TilesetContentLoaderResult<EllipsoidTerrainLoader> errorResult;
    errorResult.errors.emplaceError(fmt::format(
        "Tileset layer.json contained an unknown projection value: {}",
        projectionString));
    return externals.asyncSystem.createResolvedFuture(std::move(errorResult));
  }

  BoundingVolume boundingVolume =
      createDefaultLooseEarthBoundingVolume(quadtreeRectangleGlobe);

  CesiumGeometry::QuadtreeTilingScheme tilingScheme(
      quadtreeRectangleProjected,
      quadtreeXTiles,
      1);

  auto pLoader =
      std::make_unique<EllipsoidTerrainLoader>(tilingScheme, projection);

  std::unique_ptr<Tile> pRootTile =
      std::make_unique<Tile>(pLoader.get(), TileEmptyContent());
  pRootTile->setUnconditionallyRefine();
  pRootTile->setBoundingVolume(boundingVolume);

  std::vector<Tile> childTiles;
  childTiles.reserve(quadtreeXTiles);

  for (uint32_t i = 0; i < quadtreeXTiles; ++i) {
    Tile& childTile = childTiles.emplace_back(pLoader.get());

    QuadtreeTileID id(0, i, 0);
    childTile.setTileID(id);

    const CesiumGeospatial::GlobeRectangle childGlobeRectangle =
        unprojectRectangleSimple(projection, tilingScheme.tileToRectangle(id));
    childTile.setBoundingVolume(
        createDefaultLooseEarthBoundingVolume(childGlobeRectangle));
    childTile.setGeometricError(
        8.0 * calcQuadtreeMaxGeometricError(Ellipsoid::WGS84) *
        childGlobeRectangle.computeWidth());
  }

  pRootTile->createChildTiles(std::move(childTiles));

  std::vector<LoaderCreditResult> credits;
  ErrorList errors;
  TilesetContentLoaderResult<EllipsoidTerrainLoader> finalResult{
      std::move(pLoader),
      std::move(pRootTile),
      std::move(credits),
      std::vector<IAssetAccessor::THeader>{},
      std::move(errors)};

  return externals.asyncSystem.createResolvedFuture(std::move(finalResult));
}

EllipsoidTerrainLoader::EllipsoidTerrainLoader(
    const CesiumGeometry::QuadtreeTilingScheme& tilingScheme,
    const CesiumGeospatial::Projection& projection)
    : _tilingScheme(tilingScheme), _projection(projection) {}
CesiumAsync::Future<TileLoadResult>
EllipsoidTerrainLoader::loadTileContent(const TileLoadInput& loadInput) {
  const auto& tile = loadInput.tile;
  const auto& asyncSystem = loadInput.asyncSystem;

  // This type of loader should never have child loaders.
  assert(tile.getLoader() == this);

  Future<QuantizedMeshLoadResult> futureQuantizedMesh =
      asyncSystem.runInWorkerThread([&tile]() {
        const QuadtreeTileID* pQuadtreeTileID =
            std::get_if<QuadtreeTileID>(&tile.getTileID());

        const BoundingRegion* pRegion =
            getBoundingRegionFromBoundingVolume(tile.getBoundingVolume());

        return QuantizedMeshLoader::createGridMesh(
            *pQuadtreeTileID,
            *pRegion,
            33,
            33);
      });

  return std::move(futureQuantizedMesh)
      .thenInMainThread(
          [this, asyncSystem, &tile](QuantizedMeshLoadResult&& loadResult) {
            auto finalResult = convertToTileLoadResult(std::move(loadResult));
            return asyncSystem.createResolvedFuture(std::move(finalResult));
          });
}

TileChildrenResult
EllipsoidTerrainLoader::createTileChildren(const Tile& tile) {
  if (const CesiumGeometry::QuadtreeTileID* pQuadtreeID =
          std::get_if<CesiumGeometry::QuadtreeTileID>(&tile.getTileID())) {
    return {createTileChildrenImpl(tile), TileLoadResultState::Success};
  }
  return {{}, TileLoadResultState::Failed};
}

const CesiumGeometry::QuadtreeTilingScheme&
EllipsoidTerrainLoader::getTilingScheme() const noexcept {
  return this->_tilingScheme;
}

const CesiumGeospatial::Projection&
EllipsoidTerrainLoader::getProjection() const noexcept {
  return this->_projection;
}

std::vector<Tile>
EllipsoidTerrainLoader::createTileChildrenImpl(const Tile& tile) {
  const QuadtreeTileID* pQuadtreeTileID =
      std::get_if<QuadtreeTileID>(&tile.getTileID());

  // Now that all our availability is sorted out, create this tile's
  // children.
  const QuadtreeTileID swID(
      pQuadtreeTileID->level + 1,
      pQuadtreeTileID->x * 2,
      pQuadtreeTileID->y * 2);
  const QuadtreeTileID seID(swID.level, swID.x + 1, swID.y);
  const QuadtreeTileID nwID(swID.level, swID.x, swID.y + 1);
  const QuadtreeTileID neID(swID.level, swID.x + 1, swID.y + 1);

  std::vector<Tile> children;
  children.reserve(4);

  createChildTile(tile, children, swID);
  createChildTile(tile, children, seID);
  createChildTile(tile, children, nwID);
  createChildTile(tile, children, neID);

  return children;
}

void EllipsoidTerrainLoader::createChildTile(
    const Tile& parent,
    std::vector<Tile>& children,
    const CesiumGeometry::QuadtreeTileID& childID) {

  Tile& child = children.emplace_back(this);
  child.setRefine(parent.getRefine());
  child.setTransform(parent.getTransform());
  child.setTileID(childID);
  child.setGeometricError(parent.getGeometricError() * 0.5);

  const CesiumGeospatial::GlobeRectangle childGlobeRectangle =
      unprojectRectangleSimple(
          this->_projection,
          this->_tilingScheme.tileToRectangle(childID));
  child.setBoundingVolume(BoundingRegionWithLooseFittingHeights(
      BoundingRegion(childGlobeRectangle, 0.0, 0.0)));
}

} // namespace Cesium3DTilesSelection
