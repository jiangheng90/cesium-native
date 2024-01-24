#pragma once

#include "LayerJsonTerrainLoader.h"

namespace Cesium3DTilesSelection {

class EllipsoidTerrainLoader : public TilesetContentLoader {
  enum class AvailableState { Available, NotAvailable, Unknown };

public:
  inline static const std::string EPSG4326{"EPSG:4326"};
  inline static const std::string EPSG3857{"EPSG:3857"};

  static bool isProjectionString(const std::string& str);

  static CesiumAsync::Future<TilesetContentLoaderResult<EllipsoidTerrainLoader>>
  createLoader(
      const TilesetExternals& externals,
      const std::string& projectionString);

  EllipsoidTerrainLoader(
      const CesiumGeometry::QuadtreeTilingScheme& tilingScheme,
      const CesiumGeospatial::Projection& projection);

  CesiumAsync::Future<TileLoadResult>
  loadTileContent(const TileLoadInput& loadInput) override;

  TileChildrenResult createTileChildren(const Tile& tile) override;

  const CesiumGeometry::QuadtreeTilingScheme& getTilingScheme() const noexcept;

  const CesiumGeospatial::Projection& getProjection() const noexcept;

private:
  std::vector<Tile> createTileChildrenImpl(const Tile& tile);

  void createChildTile(
      const Tile& parent,
      std::vector<Tile>& children,
      const CesiumGeometry::QuadtreeTileID& childID);

  CesiumGeometry::QuadtreeTilingScheme _tilingScheme;
  CesiumGeospatial::Projection _projection;
};

} // namespace Cesium3DTilesSelection
