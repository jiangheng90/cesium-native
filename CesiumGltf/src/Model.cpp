#include "CesiumGltf/Model.h"
#include "CesiumGltf/AccessorView.h"
#include "CesiumGltf/KHR_draco_mesh_compression.h"
#include <algorithm>
#include <glm/vec3.hpp>
#include <gsl/span>

using namespace CesiumGltf;

namespace {
template <typename T>
size_t copyElements(std::vector<T>& to, std::vector<T>& from) {
  size_t out = to.size();
  to.resize(out + from.size());
  for (size_t i = 0; i < from.size(); ++i) {
    to[out + i] = std::move(from[i]);
  }

  return out;
}

void updateIndex(int32_t& index, size_t offset) {
  if (index == -1) {
    return;
  }
  index += int32_t(offset);
}
} // namespace

void Model::merge(Model&& rhs) {
  // TODO: we could generate this pretty easily if the glTF JSON schema made
  // it clear which index properties refer to which types of objects.

  // Copy all the source data into this instance.
  copyElements(this->extensionsUsed, rhs.extensionsUsed);
  std::sort(this->extensionsUsed.begin(), this->extensionsUsed.end());
  this->extensionsUsed.erase(
      std::unique(this->extensionsUsed.begin(), this->extensionsUsed.end()),
      this->extensionsUsed.end());

  copyElements(this->extensionsRequired, rhs.extensionsRequired);
  std::sort(this->extensionsRequired.begin(), this->extensionsRequired.end());
  this->extensionsRequired.erase(
      std::unique(
          this->extensionsRequired.begin(),
          this->extensionsRequired.end()),
      this->extensionsRequired.end());

  size_t firstAccessor = copyElements(this->accessors, rhs.accessors);
  size_t firstAnimation = copyElements(this->animations, rhs.animations);
  size_t firstBuffer = copyElements(this->buffers, rhs.buffers);
  size_t firstBufferView = copyElements(this->bufferViews, rhs.bufferViews);
  size_t firstCamera = copyElements(this->cameras, rhs.cameras);
  size_t firstImage = copyElements(this->images, rhs.images);
  size_t firstMaterial = copyElements(this->materials, rhs.materials);
  size_t firstMesh = copyElements(this->meshes, rhs.meshes);
  size_t firstNode = copyElements(this->nodes, rhs.nodes);
  size_t firstSampler = copyElements(this->samplers, rhs.samplers);
  size_t firstScene = copyElements(this->scenes, rhs.scenes);
  size_t firstSkin = copyElements(this->skins, rhs.skins);
  size_t firstTexture = copyElements(this->textures, rhs.textures);

  // Update the copied indices
  for (size_t i = firstAccessor; i < this->accessors.size(); ++i) {
    Accessor& accessor = this->accessors[i];
    updateIndex(accessor.bufferView, firstBufferView);

    if (accessor.sparse) {
      updateIndex(accessor.sparse.value().indices.bufferView, firstBufferView);
      updateIndex(accessor.sparse.value().values.bufferView, firstBufferView);
    }
  }

  for (size_t i = firstAnimation; i < this->animations.size(); ++i) {
    Animation& animation = this->animations[i];

    for (AnimationChannel& channel : animation.channels) {
      updateIndex(channel.sampler, firstSampler);
      updateIndex(channel.target.node, firstNode);
    }

    for (AnimationSampler& sampler : animation.samplers) {
      updateIndex(sampler.input, firstAccessor);
      updateIndex(sampler.output, firstAccessor);
    }
  }

  for (size_t i = firstBufferView; i < this->bufferViews.size(); ++i) {
    BufferView& bufferView = this->bufferViews[i];
    updateIndex(bufferView.buffer, firstBuffer);
  }

  for (size_t i = firstImage; i < this->images.size(); ++i) {
    Image& image = this->images[i];
    updateIndex(image.bufferView, firstBufferView);
  }

  for (size_t i = firstMesh; i < this->meshes.size(); ++i) {
    Mesh& mesh = this->meshes[i];

    for (MeshPrimitive& primitive : mesh.primitives) {
      updateIndex(primitive.indices, firstAccessor);
      updateIndex(primitive.material, firstMaterial);

      for (auto& attribute : primitive.attributes) {
        updateIndex(attribute.second, firstAccessor);
      }

      for (auto& target : primitive.targets) {
        for (auto& displacement : target) {
          updateIndex(displacement.second, firstAccessor);
        }
      }

      KHR_draco_mesh_compression* pDraco =
          primitive.getExtension<KHR_draco_mesh_compression>();
      if (pDraco) {
        updateIndex(pDraco->bufferView, firstBufferView);
      }
    }
  }

  for (size_t i = firstNode; i < this->nodes.size(); ++i) {
    Node& node = this->nodes[i];

    updateIndex(node.camera, firstCamera);
    updateIndex(node.skin, firstSkin);
    updateIndex(node.mesh, firstMesh);

    for (auto& nodeIndex : node.children) {
      updateIndex(nodeIndex, firstNode);
    }
  }

  for (size_t i = firstScene; i < this->scenes.size(); ++i) {
    Scene& currentScene = this->scenes[i];
    for (int32_t& node : currentScene.nodes) {
      updateIndex(node, firstNode);
    }
  }

  for (size_t i = firstSkin; i < this->skins.size(); ++i) {
    Skin& skin = this->skins[i];

    updateIndex(skin.inverseBindMatrices, firstAccessor);
    updateIndex(skin.skeleton, firstNode);

    for (int32_t& nodeIndex : skin.joints) {
      updateIndex(nodeIndex, firstNode);
    }
  }

  for (size_t i = firstTexture; i < this->textures.size(); ++i) {
    Texture& texture = this->textures[i];

    updateIndex(texture.sampler, firstSampler);
    updateIndex(texture.source, firstImage);
  }

  for (size_t i = firstMaterial; i < this->materials.size(); ++i) {
    Material& material = this->materials[i];

    if (material.normalTexture) {
      updateIndex(material.normalTexture.value().index, firstTexture);
    }
    if (material.occlusionTexture) {
      updateIndex(material.occlusionTexture.value().index, firstTexture);
    }
    if (material.pbrMetallicRoughness) {
      MaterialPBRMetallicRoughness& pbr = material.pbrMetallicRoughness.value();
      if (pbr.baseColorTexture) {
        updateIndex(pbr.baseColorTexture.value().index, firstTexture);
      }
      if (pbr.metallicRoughnessTexture) {
        updateIndex(pbr.metallicRoughnessTexture.value().index, firstTexture);
      }
    }
    if (material.emissiveTexture) {
      updateIndex(material.emissiveTexture.value().index, firstTexture);
    }
  }

  Scene* pThisDefaultScene = Model::getSafe(&this->scenes, this->scene);
  Scene* pRhsDefaultScene =
      Model::getSafe(&this->scenes, rhs.scene + int32_t(firstScene));

  if (!pThisDefaultScene) {
    this->scene = rhs.scene;
    updateIndex(this->scene, firstScene);
  } else if (pRhsDefaultScene) {
    // Create a new default scene that has all the root nodes in
    // the default scene of either model.
    Scene& newScene = this->scenes.emplace_back();

    // Refresh the scene pointers potentially invalidated by the above.
    pThisDefaultScene = Model::getSafe(&this->scenes, this->scene);
    pRhsDefaultScene =
        Model::getSafe(&this->scenes, rhs.scene + int32_t(firstScene));

    newScene.nodes = pThisDefaultScene->nodes;
    size_t originalNodeCount = newScene.nodes.size();
    newScene.nodes.resize(originalNodeCount + pRhsDefaultScene->nodes.size());
    std::copy(
        pRhsDefaultScene->nodes.begin(),
        pRhsDefaultScene->nodes.end(),
        newScene.nodes.begin() + int64_t(originalNodeCount));

    // No need to update indices because they've already been updated when
    // we copied them from rhs to this.

    this->scene = int32_t(this->scenes.size() - 1);
  }
}

namespace {
// Initialize with a static function instead of inline to avoid an
// internal compiler error in MSVC v14.27.29110.
glm::dmat4 createGltfAxesToCesiumAxes() {
  // https://github.com/CesiumGS/3d-tiles/tree/master/specification#gltf-transforms
  return glm::dmat4(
      glm::dvec4(1.0, 0.0, 0.0, 0.0),
      glm::dvec4(0.0, 0.0, 1.0, 0.0),
      glm::dvec4(0.0, -1.0, 0.0, 0.0),
      glm::dvec4(0.0, 0.0, 0.0, 1.0));
}

glm::dmat4 gltfAxesToCesiumAxes = createGltfAxesToCesiumAxes();

void forEachPrimitiveInMeshObject(
    const glm::dmat4x4& transform,
    const Model& model,
    const Node& node,
    const Mesh& mesh,
    std::function<Model::ForEachPrimitiveInSceneConstCallback>& callback) {
  for (const MeshPrimitive& primitive : mesh.primitives) {
    callback(model, node, mesh, primitive, transform);
  }
}

void forEachPrimitiveInNodeObject(
    const glm::dmat4x4& transform,
    const Model& model,
    const Node& node,
    std::function<Model::ForEachPrimitiveInSceneConstCallback>& callback) {
  glm::dmat4x4 nodeTransform = transform;

  if (!node.matrix.empty()) {
    const std::vector<double>& matrix = node.matrix;

    glm::dmat4x4 nodeTransformGltf(
        glm::dvec4(matrix[0], matrix[1], matrix[2], matrix[3]),
        glm::dvec4(matrix[4], matrix[5], matrix[6], matrix[7]),
        glm::dvec4(matrix[8], matrix[9], matrix[10], matrix[11]),
        glm::dvec4(matrix[12], matrix[13], matrix[14], matrix[15]));

    nodeTransform = nodeTransform * nodeTransformGltf;
  } else if (
      !node.translation.empty() || !node.rotation.empty() ||
      !node.scale.empty()) {
    // TODO: handle this type of transformation
  }

  int meshId = node.mesh;
  if (meshId >= 0 && meshId < static_cast<int>(model.meshes.size())) {
    const Mesh& mesh = model.meshes[static_cast<size_t>(meshId)];
    forEachPrimitiveInMeshObject(nodeTransform, model, node, mesh, callback);
  }

  for (int childNodeId : node.children) {
    if (childNodeId >= 0 &&
        childNodeId < static_cast<int>(model.nodes.size())) {
      forEachPrimitiveInNodeObject(
          nodeTransform,
          model,
          model.nodes[static_cast<size_t>(childNodeId)],
          callback);
    }
  }
}

void forEachPrimitiveInSceneObject(
    const glm::dmat4x4& transform,
    const Model& model,
    const Scene& scene,
    std::function<Model::ForEachPrimitiveInSceneConstCallback>& callback) {
  for (int nodeID : scene.nodes) {
    if (nodeID >= 0 && nodeID < static_cast<int>(model.nodes.size())) {
      forEachPrimitiveInNodeObject(
          transform,
          model,
          model.nodes[static_cast<size_t>(nodeID)],
          callback);
    }
  }
}
} // namespace

void Model::forEachPrimitiveInScene(
    int sceneID,
    std::function<ForEachPrimitiveInSceneCallback>&& callback) {
  return const_cast<const Model*>(this)->forEachPrimitiveInScene(
      sceneID,
      [&callback](
          const Model& gltf_,
          const Node& node,
          const Mesh& mesh,
          const MeshPrimitive& primitive,
          const glm::dmat4& transform) {
        callback(
            const_cast<Model&>(gltf_),
            const_cast<Node&>(node),
            const_cast<Mesh&>(mesh),
            const_cast<MeshPrimitive&>(primitive),
            transform);
      });
}

void Model::forEachPrimitiveInScene(
    int sceneID,
    std::function<ForEachPrimitiveInSceneConstCallback>&& callback) const {
  glm::dmat4x4 rootTransform(1.0);

  if (sceneID >= 0) {
    // Use the user-specified scene if it exists.
    if (sceneID < static_cast<int>(this->scenes.size())) {
      forEachPrimitiveInSceneObject(
          rootTransform,
          *this,
          this->scenes[static_cast<size_t>(sceneID)],
          callback);
    }
  } else if (
      this->scene >= 0 &&
      this->scene < static_cast<int32_t>(this->scenes.size())) {
    // Use the default scene
    forEachPrimitiveInSceneObject(
        rootTransform,
        *this,
        this->scenes[static_cast<size_t>(this->scene)],
        callback);
  } else if (!this->scenes.empty()) {
    // There's no default, so use the first scene
    const Scene& defaultScene = this->scenes[0];
    forEachPrimitiveInSceneObject(rootTransform, *this, defaultScene, callback);
  } else if (!this->nodes.empty()) {
    // No scenes at all, use the first node as the root node.
    forEachPrimitiveInNodeObject(
        rootTransform,
        *this,
        this->nodes[0],
        callback);
  } else if (!this->meshes.empty()) {
    // No nodes either, show all the meshes.
    for (const Mesh& mesh : this->meshes) {
      forEachPrimitiveInMeshObject(
          rootTransform,
          *this,
          Node(),
          mesh,
          callback);
    }
  }
}

namespace {
template <typename TIndex>
static void generateSmoothNormals(
    Model& gltf,
    MeshPrimitive& primitive,
    const AccessorView<glm::vec3>& positionView,
    const std::optional<Accessor>& indexAccessor) {

  size_t count = static_cast<size_t>(positionView.size());
  size_t normalBufferStride = sizeof(glm::vec3);
  size_t normalBufferSize = count * normalBufferStride;

  std::vector<std::byte> normalByteBuffer(normalBufferSize);
  gsl::span<glm::vec3> normals(
      reinterpret_cast<glm::vec3*>(normalByteBuffer.data()),
      count);

  // Add the triangle's normal to each of it's vertex's accumulated normal.
  auto addTriangleNormalToVertexNormals =
      [&positionView,
       &normals](TIndex tIndex0, TIndex tIndex1, TIndex tIndex2) -> void {
    size_t index0 = static_cast<size_t>(tIndex0);
    size_t index1 = static_cast<size_t>(tIndex1);
    size_t index2 = static_cast<size_t>(tIndex2);

    const glm::vec3& vertex0 = positionView[static_cast<int64_t>(index0)];
    const glm::vec3& vertex1 = positionView[static_cast<int64_t>(index1)];
    const glm::vec3& vertex2 = positionView[static_cast<int64_t>(index2)];

    glm::vec3 triangleNormal = glm::cross(vertex1 - vertex0, vertex2 - vertex0);

    // Add the triangle normal to each vertex's accumulated normal. At the end
    // we will normalize the accumulated vertex normals to average.
    normals[index0] += triangleNormal;
    normals[index1] += triangleNormal;
    normals[index2] += triangleNormal;
  };

  int64_t numIndices;
  std::function<TIndex(int64_t)> getIndex;

  // In the indexed case, the positions are accessed with the
  // indices from the indexView. Otherwise, the elements are
  // accessed directly.
  if (indexAccessor) {
    CesiumGltf::AccessorView<TIndex> indexView(gltf, *indexAccessor);
    if (indexView.status() != AccessorViewStatus::Valid) {
      return;
    }
    numIndices = indexView.size();
    getIndex = [&indexView](int64_t index) { return indexView[index]; };
  } else {
    numIndices = int64_t(count);
    getIndex = [](int64_t index) { return static_cast<TIndex>(index); };
  }

  switch (primitive.mode) {
  case MeshPrimitive::Mode::TRIANGLES:
    for (int64_t i = 2; i < numIndices; i += 3) {
      TIndex index0 = getIndex(i - 2);
      TIndex index1 = getIndex(i - 1);
      TIndex index2 = getIndex(i);

      addTriangleNormalToVertexNormals(index0, index1, index2);
    }
    break;

  case MeshPrimitive::Mode::TRIANGLE_STRIP:
    for (int64_t i = 0; i < numIndices - 2; ++i) {
      TIndex index0;
      TIndex index1;
      TIndex index2;

      if (i % 2) {
        index0 = getIndex(i);
        index1 = getIndex(i + 2);
        index2 = getIndex(i + 1);
      } else {
        index0 = getIndex(i);
        index1 = getIndex(i + 1);
        index2 = getIndex(i + 2);
      }

      addTriangleNormalToVertexNormals(index0, index1, index2);
    }
    break;

  case MeshPrimitive::Mode::TRIANGLE_FAN:
    if (numIndices < 3) {
      return;
    }

    {
      TIndex index0 = getIndex(0);
      for (int64_t i = 2; i < numIndices; ++i) {
        TIndex index1 = getIndex(i - 1);
        TIndex index2 = getIndex(i);

        addTriangleNormalToVertexNormals(index0, index1, index2);
      }
    }
    break;

  default:
    return;
  }

  // normalizes the accumulated vertex normals
  for (size_t i = 0; i < count; ++i) {
    normals[i] = glm::normalize(normals[i]);
  }

  size_t normalBufferId = gltf.buffers.size();
  Buffer& normalBuffer = gltf.buffers.emplace_back();
  normalBuffer.byteLength = static_cast<int64_t>(normalBufferSize);
  normalBuffer.cesium.data = std::move(normalByteBuffer);

  size_t normalBufferViewId = gltf.bufferViews.size();
  BufferView& normalBufferView = gltf.bufferViews.emplace_back();
  normalBufferView.buffer = static_cast<int32_t>(normalBufferId);
  normalBufferView.byteLength = static_cast<int64_t>(normalBufferSize);
  normalBufferView.byteOffset = 0;
  normalBufferView.byteStride = static_cast<int64_t>(normalBufferStride);
  normalBufferView.target = BufferView::Target::ARRAY_BUFFER;

  size_t normalAccessorId = gltf.accessors.size();
  Accessor& normalAccessor = gltf.accessors.emplace_back();
  normalAccessor.byteOffset = 0;
  normalAccessor.bufferView = static_cast<int32_t>(normalBufferViewId);
  normalAccessor.componentType = Accessor::ComponentType::FLOAT;
  normalAccessor.count = positionView.size();
  normalAccessor.type = Accessor::Type::VEC3;

  primitive.attributes.emplace(
      "NORMAL",
      static_cast<int32_t>(normalAccessorId));
}

void generateSmoothNormals(
    Model& gltf,
    MeshPrimitive& primitive,
    const AccessorView<glm::vec3>& positionView,
    const std::optional<Accessor>& indexAccessor) {
  if (indexAccessor) {
    switch (indexAccessor->componentType) {
    case Accessor::ComponentType::BYTE:
      generateSmoothNormals<int8_t>(
          gltf,
          primitive,
          positionView,
          indexAccessor);
      break;
    case Accessor::ComponentType::UNSIGNED_BYTE:
      generateSmoothNormals<uint8_t>(
          gltf,
          primitive,
          positionView,
          indexAccessor);
      break;
    case Accessor::ComponentType::SHORT:
      generateSmoothNormals<int16_t>(
          gltf,
          primitive,
          positionView,
          indexAccessor);
      break;
    case Accessor::ComponentType::UNSIGNED_SHORT:
      generateSmoothNormals<uint16_t>(
          gltf,
          primitive,
          positionView,
          indexAccessor);
      break;
    case Accessor::ComponentType::UNSIGNED_INT:
      generateSmoothNormals<uint32_t>(
          gltf,
          primitive,
          positionView,
          indexAccessor);
      break;
    case Accessor::ComponentType::FLOAT:
      return;
    };
  } else {
    generateSmoothNormals<size_t>(gltf, primitive, positionView, std::nullopt);
  }
}
} // namespace

void Model::generateMissingNormalsSmooth() {
  forEachPrimitiveInScene(
      -1,
      [](Model& gltf_,
         Node& /*node*/,
         Mesh& /*mesh*/,
         MeshPrimitive& primitive,
         const glm::dmat4& /*transform*/) {
        // if normals already exist, there is nothing to do
        auto normalIt = primitive.attributes.find("NORMAL");
        if (normalIt != primitive.attributes.end()) {
          return;
        }

        // if positions do not exist, we cannot create normals.
        auto positionIt = primitive.attributes.find("POSITION");
        if (positionIt == primitive.attributes.end()) {
          return;
        }

        int positionAccessorId = positionIt->second;
        AccessorView<glm::vec3> positionView(gltf_, positionAccessorId);
        if (positionView.status() != AccessorViewStatus::Valid) {
          return;
        }

        if (primitive.indices < 0 ||
            static_cast<size_t>(primitive.indices) >= gltf_.accessors.size()) {
          generateSmoothNormals(gltf_, primitive, positionView, std::nullopt);
        } else {
          Accessor& indexAccessor =
              gltf_.accessors[static_cast<size_t>(primitive.indices)];
          generateSmoothNormals(gltf_, primitive, positionView, indexAccessor);
        }
      });
}