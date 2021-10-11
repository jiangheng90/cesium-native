// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "Library.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <vector>

using namespace CesiumUtility;

namespace Cesium3DTiles {
/**
 * @brief A bounding volume that encloses a tile or its content. Exactly one
 * `box`, `region`, or `sphere` property is required.
 */
struct CESIUM3DTILES_API BoundingVolume final : public ExtensibleObject {
  static inline constexpr const char* TypeName = "BoundingVolume";

  /**
   * @brief An array of 12 numbers that define an oriented bounding box. The
   * first three elements define the x, y, and z values for the center of the
   * box. The next three elements (with indices 3, 4, and 5) define the x axis
   * direction and half-length. The next three elements (indices 6, 7, and 8)
   * define the y axis direction and half-length. The last three elements
   * (indices 9, 10, and 11) define the z axis direction and half-length.
   */
  std::vector<double> box;

  /**
   * @brief An array of six numbers that define a bounding geographic region in
   * EPSG:4979 coordinates with the order [west, south, east, north, minimum
   * height, maximum height]. Longitudes and latitudes are in radians, and
   * heights are in meters above (or below) the WGS84 ellipsoid.
   */
  std::vector<double> region;

  /**
   * @brief An array of four numbers that define a bounding sphere. The first
   * three elements define the x, y, and z values for the center of the sphere.
   * The last element (with index 3) defines the radius in meters.
   */
  std::vector<double> sphere;
};
} // namespace Cesium3DTiles
