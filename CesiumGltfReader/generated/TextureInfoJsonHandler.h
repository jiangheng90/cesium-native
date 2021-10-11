// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/ReaderContext.h"
#include "ExtensibleObjectJsonHandler.h"

#include <CesiumGltf/TextureInfo.h>
#include <CesiumJsonReader/IntegerJsonHandler.h>

namespace CesiumGltf {
struct ReaderContext;

class TextureInfoJsonHandler : public ExtensibleObjectJsonHandler {
public:
  using ValueType = TextureInfo;

  TextureInfoJsonHandler(const ReaderContext& context) noexcept;
  void reset(IJsonHandler* pParentHandler, TextureInfo* pObject);

  virtual IJsonHandler* readObjectKey(const std::string_view& str) override;

protected:
  IJsonHandler* readObjectKeyTextureInfo(
      const std::string& objectType,
      const std::string_view& str,
      TextureInfo& o);

private:
  TextureInfo* _pObject = nullptr;
  CesiumJsonReader::IntegerJsonHandler<int32_t> _index;
  CesiumJsonReader::IntegerJsonHandler<int64_t> _texCoord;
};
} // namespace CesiumGltf
