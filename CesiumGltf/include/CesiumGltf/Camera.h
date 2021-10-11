// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CameraOrthographic.h"
#include "CameraPerspective.h"
#include "Library.h"
#include "NamedObject.h"

#include <optional>

using namespace CesiumUtility;

namespace CesiumGltf {
/**
 * @brief A camera's projection.  A node **MAY** reference a camera to apply a
 * transform to place the camera in the scene.
 */
struct CESIUMGLTF_API Camera final : public NamedObject {
  static inline constexpr const char* TypeName = "Camera";

  /**
   * @brief Known values for Specifies if the camera uses a perspective or
   * orthographic projection.
   */
  struct Type {
    inline static const std::string perspective = "perspective";

    inline static const std::string orthographic = "orthographic";
  };

  /**
   * @brief An orthographic camera containing properties to create an
   * orthographic projection matrix. This property **MUST NOT** be defined when
   * `perspective` is defined.
   */
  std::optional<CameraOrthographic> orthographic;

  /**
   * @brief A perspective camera containing properties to create a perspective
   * projection matrix. This property **MUST NOT** be defined when
   * `orthographic` is defined.
   */
  std::optional<CameraPerspective> perspective;

  /**
   * @brief Specifies if the camera uses a perspective or orthographic
   * projection.
   *
   * Known values are defined in {@link Type}.
   *
   *
   * Based on this, either the camera's `perspective` or `orthographic` property
   * **MUST** be defined.
   */
  std::string type = Type::perspective;
};
} // namespace CesiumGltf
