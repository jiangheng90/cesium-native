// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include <Cesium3DTiles/Library.h>
#include <CesiumUtility/ExtensibleObject.h>
#include <CesiumUtility/JsonValue.h>

#include <optional>
#include <string>
#include <unordered_map>

namespace Cesium3DTiles {
/**
 * @brief Statistics about property values.
 */
struct CESIUM3DTILES_API PropertyStatistics final
    : public CesiumUtility::ExtensibleObject {
  /**
   * @brief The original name of this type.
   */
  static constexpr const char* TypeName = "PropertyStatistics";

  /**
   * @brief The minimum property value occurring in the tileset. Only applicable
   * to `SCALAR`, `VECN`, and `MATN` types. This is the minimum of all property
   * values, after the transforms based on the `normalized`, `offset`, and
   * `scale` properties have been applied.
   */
  std::optional<CesiumUtility::JsonValue> min;

  /**
   * @brief The maximum property value occurring in the tileset. Only applicable
   * to `SCALAR`, `VECN`, and `MATN` types. This is the maximum of all property
   * values, after the transforms based on the `normalized`, `offset`, and
   * `scale` properties have been applied.
   */
  std::optional<CesiumUtility::JsonValue> max;

  /**
   * @brief The arithmetic mean of property values occurring in the tileset.
   * Only applicable to `SCALAR`, `VECN`, and `MATN` types. This is the mean of
   * all property values, after the transforms based on the `normalized`,
   * `offset`, and `scale` properties have been applied.
   */
  std::optional<CesiumUtility::JsonValue> mean;

  /**
   * @brief The median of property values occurring in the tileset. Only
   * applicable to `SCALAR`, `VECN`, and `MATN` types. This is the median of all
   * property values, after the transforms based on the `normalized`, `offset`,
   * and `scale` properties have been applied.
   */
  std::optional<CesiumUtility::JsonValue> median;

  /**
   * @brief The standard deviation of property values occurring in the tileset.
   * Only applicable to `SCALAR`, `VECN`, and `MATN` types. This is the standard
   * deviation of all property values, after the transforms based on the
   * `normalized`, `offset`, and `scale` properties have been applied.
   */
  std::optional<CesiumUtility::JsonValue> standardDeviation;

  /**
   * @brief The variance of property values occurring in the tileset. Only
   * applicable to `SCALAR`, `VECN`, and `MATN` types. This is the variance of
   * all property values, after the transforms based on the `normalized`,
   * `offset`, and `scale` properties have been applied.
   */
  std::optional<CesiumUtility::JsonValue> variance;

  /**
   * @brief The sum of property values occurring in the tileset. Only applicable
   * to `SCALAR`, `VECN`, and `MATN` types. This is the sum of all property
   * values, after the transforms based on the `normalized`, `offset`, and
   * `scale` properties have been applied.
   */
  std::optional<CesiumUtility::JsonValue> sum;

  /**
   * @brief A dictionary, where each key corresponds to an enum `name` and each
   * value is the number of occurrences of that enum. Only applicable when
   * `type` is `ENUM`. For fixed-length arrays, this is an array of
   * component-wise occurrences.
   */
  std::unordered_map<std::string, CesiumUtility::JsonValue> occurrences;

  /**
   * @brief Calculates the size in bytes of this object, including the contents
   * of all collections, pointers, and strings. This will NOT include the size
   * of any extensions attached to the object. Calling this method may be slow
   * as it requires traversing the object's entire structure.
   */
  int64_t getSizeBytes() const {
    int64_t accum = 0;
    accum += int64_t(sizeof(PropertyStatistics));
    accum += CesiumUtility::ExtensibleObject::getSizeBytes() -
             int64_t(sizeof(CesiumUtility::ExtensibleObject));
    accum += int64_t(
        this->occurrences.bucket_count() *
        (sizeof(std::string) + sizeof(CesiumUtility::JsonValue)));
    for (const auto& [k, v] : this->occurrences) {
      accum += int64_t(k.capacity() * sizeof(char) - sizeof(std::string));
      accum += int64_t(sizeof(CesiumUtility::JsonValue));
    }
    return accum;
  }
};
} // namespace Cesium3DTiles
