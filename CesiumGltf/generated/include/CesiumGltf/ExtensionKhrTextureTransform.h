// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/Library.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <cstdint>
#include <optional>
#include <vector>

namespace CesiumGltf {
/**
 * @brief glTF extension that enables shifting and scaling UV coordinates on a
 * per-texture basis
 */
struct CESIUMGLTF_API ExtensionKhrTextureTransform final
    : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName = "ExtensionKhrTextureTransform";
  static inline constexpr const char* ExtensionName = "KHR_texture_transform";

  /**
   * @brief The offset of the UV coordinate origin as a factor of the texture
   * dimensions.
   */
  std::vector<double> offset = {0, 0};

  /**
   * @brief Rotate the UVs by this many radians counter-clockwise around the
   * origin.
   */
  double rotation = 0;

  /**
   * @brief The scale factor applied to the components of the UV coordinates.
   */
  std::vector<double> scale = {1, 1};

  /**
   * @brief Overrides the textureInfo texCoord value if supplied, and if this
   * extension is supported.
   */
  std::optional<int64_t> texCoord;
};
} // namespace CesiumGltf