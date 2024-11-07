// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/Library.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <cstdint>
#include <vector>

namespace CesiumGltf {
/**
 * @brief Structural metadata about a glTF primitive.
 */
struct CESIUMGLTF_API ExtensionMeshPrimitiveExtStructuralMetadata final
    : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName =
      "ExtensionMeshPrimitiveExtStructuralMetadata";
  static inline constexpr const char* ExtensionName = "EXT_structural_metadata";

  /**
   * @brief An array of indexes of property textures in the root
   * `EXT_structural_metadata` object.
   */
  std::vector<int32_t> propertyTextures;

  /**
   * @brief An array of indexes of property attributes in the root
   * `EXT_structural_metadata` object.
   */
  std::vector<int32_t> propertyAttributes;

  int64_t getSizeBytes() const {
    int64_t accum = 0;
    accum += sizeof(ExtensionMeshPrimitiveExtStructuralMetadata);
    accum += sizeof(int32_t) * this->propertyTextures.capacity();
    accum += sizeof(int32_t) * this->propertyAttributes.capacity();
    return accum;
  }
};
} // namespace CesiumGltf
