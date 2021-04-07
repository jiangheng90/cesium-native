// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "ArrayJsonHandler.h"
#include "DoubleJsonHandler.h"
#include "ExtensibleObjectJsonHandler.h"
#include "TextureInfoJsonHandler.h"
#include <CesiumGltf/Reader.h>

namespace CesiumGltf {
struct MaterialPBRMetallicRoughness;

class MaterialPBRMetallicRoughnessJsonHandler
    : public ExtensibleObjectJsonHandler {
public:
  MaterialPBRMetallicRoughnessJsonHandler(
      const ReadModelOptions& options) noexcept;
  void reset(IJsonHandler* pHandler, MaterialPBRMetallicRoughness* pObject);
  MaterialPBRMetallicRoughness* getObject();
  virtual void reportWarning(
      const std::string& warning,
      std::vector<std::string>&& context = std::vector<std::string>()) override;

  virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

protected:
  IJsonHandler* MaterialPBRMetallicRoughnessKey(
      const std::string& objectType,
      const std::string_view& str,
      MaterialPBRMetallicRoughness& o);

private:
  MaterialPBRMetallicRoughness* _pObject = nullptr;
  ArrayJsonHandler<double, DoubleJsonHandler> _baseColorFactor;
  TextureInfoJsonHandler _baseColorTexture;
  DoubleJsonHandler _metallicFactor;
  DoubleJsonHandler _roughnessFactor;
  TextureInfoJsonHandler _metallicRoughnessTexture;
};
} // namespace CesiumGltf
