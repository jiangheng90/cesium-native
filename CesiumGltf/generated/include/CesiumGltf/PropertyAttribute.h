// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/Library.h"
#include "CesiumGltf/PropertyAttributeProperty.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <optional>
#include <string>
#include <unordered_map>

namespace CesiumGltf {
/**
 * @brief Properties conforming to a class, organized as property values stored
 * in attributes.
 */
struct CESIUMGLTF_API PropertyAttribute final
    : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName = "PropertyAttribute";

  /**
   * @brief The name of the property attribute, e.g. for display purposes.
   */
  std::optional<std::string> name;

  /**
   * @brief The class that property values conform to. The value must be a class
   * ID declared in the `classes` dictionary.
   */
  std::string classProperty;

  /**
   * @brief A dictionary, where each key corresponds to a property ID in the
   * class' `properties` dictionary and each value is an object describing where
   * property values are stored. Required properties must be included in this
   * dictionary.
   */
  std::unordered_map<std::string, CesiumGltf::PropertyAttributeProperty>
      properties;
};
} // namespace CesiumGltf