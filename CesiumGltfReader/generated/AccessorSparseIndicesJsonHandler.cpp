// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#include "AccessorSparseIndicesJsonHandler.h"
#include "CesiumGltf/AccessorSparseIndices.h"

#include <cassert>
#include <string>

using namespace CesiumGltf;

AccessorSparseIndicesJsonHandler::AccessorSparseIndicesJsonHandler(
    const ReadModelOptions& options) noexcept
    : ExtensibleObjectJsonHandler(options),
      _bufferView(options),
      _byteOffset(options),
      _componentType(options) {}

void AccessorSparseIndicesJsonHandler::reset(
    IJsonHandler* pParent,
    AccessorSparseIndices* pObject) {
  ExtensibleObjectJsonHandler::reset(pParent, pObject);
  this->_pObject = pObject;
}

AccessorSparseIndices* AccessorSparseIndicesJsonHandler::getObject() {
  return this->_pObject;
}

void AccessorSparseIndicesJsonHandler::reportWarning(
    const std::string& warning,
    std::vector<std::string>&& context) {
  if (this->getCurrentKey()) {
    context.emplace_back(std::string(".") + this->getCurrentKey());
  }
  this->parent()->reportWarning(warning, std::move(context));
}

IJsonHandler*
AccessorSparseIndicesJsonHandler::readObjectKey(const std::string_view& str) {
  assert(this->_pObject);
  return this->AccessorSparseIndicesKey(
      AccessorSparseIndices::TypeName,
      str,
      *this->_pObject);
}

IJsonHandler* AccessorSparseIndicesJsonHandler::AccessorSparseIndicesKey(
    const std::string& objectType,
    const std::string_view& str,
    AccessorSparseIndices& o) {
  using namespace std::string_literals;

  if ("bufferView"s == str)
    return property("bufferView", this->_bufferView, o.bufferView);
  if ("byteOffset"s == str)
    return property("byteOffset", this->_byteOffset, o.byteOffset);
  if ("componentType"s == str)
    return property("componentType", this->_componentType, o.componentType);

  return this->ExtensibleObjectKey(objectType, str, *this->_pObject);
}
