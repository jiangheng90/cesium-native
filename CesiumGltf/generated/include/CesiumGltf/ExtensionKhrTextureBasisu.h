// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include <CesiumGltf/Library.h>
#include <CesiumUtility/ExtensibleObject.h>

#include <cstdint>

namespace CesiumGltf {
/**
 * @brief glTF extension to specify textures using the KTX v2 images with Basis
 * Universal supercompression.
 */
struct CESIUMGLTF_API ExtensionKhrTextureBasisu final
    : public CesiumUtility::ExtensibleObject {
  /**
   * @brief The original name of this type.
   */
  static constexpr const char* TypeName = "ExtensionKhrTextureBasisu";
  /** @brief The official name of the extension. This should be the same as its
   * key in the `extensions` object. */
  static constexpr const char* ExtensionName = "KHR_texture_basisu";

  /**
   * @brief The index of the image which points to a KTX v2 resource with Basis
   * Universal supercompression.
   */
  int32_t source = -1;

  /**
   * @brief Calculates the size in bytes of this object, including the contents
   * of all collections, pointers, and strings. This will NOT include the size
   * of any extensions attached to the object. Calling this method may be slow
   * as it requires traversing the object's entire structure.
   */
  int64_t getSizeBytes() const {
    int64_t accum = 0;
    accum += int64_t(sizeof(ExtensionKhrTextureBasisu));
    accum += CesiumUtility::ExtensibleObject::getSizeBytes() -
             int64_t(sizeof(CesiumUtility::ExtensibleObject));

    return accum;
  }
};
} // namespace CesiumGltf
