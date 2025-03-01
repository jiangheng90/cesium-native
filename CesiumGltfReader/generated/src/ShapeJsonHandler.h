// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "BoxJsonHandler.h"
#include "CapsuleJsonHandler.h"
#include "CylinderJsonHandler.h"
#include "NamedObjectJsonHandler.h"
#include "SphereJsonHandler.h"

#include <CesiumGltf/Shape.h>
#include <CesiumJsonReader/StringJsonHandler.h>

namespace CesiumJsonReader {
class JsonReaderOptions;
} // namespace CesiumJsonReader

namespace CesiumGltfReader {
class ShapeJsonHandler : public CesiumGltfReader::NamedObjectJsonHandler {
public:
  using ValueType = CesiumGltf::Shape;

  explicit ShapeJsonHandler(
      const CesiumJsonReader::JsonReaderOptions& options) noexcept;
  void reset(IJsonHandler* pParentHandler, CesiumGltf::Shape* pObject);

  IJsonHandler* readObjectKey(const std::string_view& str) override;

protected:
  IJsonHandler* readObjectKeyShape(
      const std::string& objectType,
      const std::string_view& str,
      CesiumGltf::Shape& o);

private:
  CesiumGltf::Shape* _pObject = nullptr;
  CesiumJsonReader::StringJsonHandler _type;
  SphereJsonHandler _sphere;
  BoxJsonHandler _box;
  CapsuleJsonHandler _capsule;
  CylinderJsonHandler _cylinder;
};
} // namespace CesiumGltfReader
