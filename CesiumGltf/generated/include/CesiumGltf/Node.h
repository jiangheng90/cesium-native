// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/Library.h"
#include "CesiumGltf/NamedObject.h"

#include <cstdint>
#include <vector>

namespace CesiumGltf {
/**
 * @brief A node in the node hierarchy.  When the node contains `skin`, all
 * `mesh.primitives` **MUST** contain `JOINTS_0` and `WEIGHTS_0` attributes.  A
 * node **MAY** have either a `matrix` or any combination of
 * `translation`/`rotation`/`scale` (TRS) properties. TRS properties are
 * converted to matrices and postmultiplied in the `T * R * S` order to compose
 * the transformation matrix; first the scale is applied to the vertices, then
 * the rotation, and then the translation. If none are provided, the transform
 * is the identity. When a node is targeted for animation (referenced by an
 * animation.channel.target), `matrix` **MUST NOT** be present.
 */
struct CESIUMGLTF_API Node final : public CesiumGltf::NamedObject {
  static inline constexpr const char* TypeName = "Node";

  /**
   * @brief The index of the camera referenced by this node.
   */
  int32_t camera = -1;

  /**
   * @brief The indices of this node's children.
   */
  std::vector<int32_t> children;

  /**
   * @brief The index of the skin referenced by this node.
   *
   * When a skin is referenced by a node within a scene, all joints used by the
   * skin **MUST** belong to the same scene. When defined, `mesh` **MUST** also
   * be defined.
   */
  int32_t skin = -1;

  /**
   * @brief A floating-point 4x4 transformation matrix stored in column-major
   * order.
   */
  std::vector<double> matrix = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

  /**
   * @brief The index of the mesh in this node.
   */
  int32_t mesh = -1;

  /**
   * @brief The node's unit quaternion rotation in the order (x, y, z, w), where
   * w is the scalar.
   */
  std::vector<double> rotation = {0, 0, 0, 1};

  /**
   * @brief The node's non-uniform scale, given as the scaling factors along the
   * x, y, and z axes.
   */
  std::vector<double> scale = {1, 1, 1};

  /**
   * @brief The node's translation along the x, y, and z axes.
   */
  std::vector<double> translation = {0, 0, 0};

  /**
   * @brief The weights of the instantiated morph target. The number of array
   * elements **MUST** match the number of morph targets of the referenced mesh.
   * When defined, `mesh` **MUST** also be defined.
   */
  std::vector<double> weights;

  int64_t getSizeBytes() const {
    int64_t accum = 0;
    accum += sizeof(Node);
    accum += sizeof(int32_t) * this->children.capacity();
    accum += sizeof(double) * this->matrix.capacity();
    accum += sizeof(double) * this->rotation.capacity();
    accum += sizeof(double) * this->scale.capacity();
    accum += sizeof(double) * this->translation.capacity();
    accum += sizeof(double) * this->weights.capacity();
    return accum;
  }
};
} // namespace CesiumGltf
