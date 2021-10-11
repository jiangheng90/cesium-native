// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "FeatureIDs.h"
#include "Library.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <string>

using namespace CesiumUtility;

namespace CesiumGltf {
/**
 * @brief An object mapping feature IDs to a feature table.
 */
struct CESIUMGLTF_API FeatureIDAttribute final : public ExtensibleObject {
  static inline constexpr const char* TypeName = "FeatureIDAttribute";

  /**
   * @brief The ID of the feature table in the model's root
   * `EXT_feature_metadata.featureTables` dictionary.
   */
  std::string featureTable;

  /**
   * @brief An object describing feature IDs to be used as indices to property
   * arrays in the feature table. Feature IDs must be whole numbers in the range
   * `[0, count - 1]` (inclusive), where `count` is the total number of features
   * in the feature table.
   */
  FeatureIDs featureIds;
};
} // namespace CesiumGltf
