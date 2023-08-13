#include "DataNode.h"

#include "Assert.h"

using namespace OSK;
using namespace OSK::MEMORY;
using namespace OSK::PERSISTENCE;

DataNode::DataNode(const SchemaId& schemaId) : m_schemaId(schemaId) {

}

const SchemaId& DataNode::GetSchemaId() const {
	return m_schemaId;
}

TDataType DataNode::GetDataType(std::string_view name) const {
	return m_dataTypesMap.at(static_cast<std::string>(name));
}

bool DataNode::HasElement(std::string_view name) const {
	return m_offsetsMap.contains(static_cast<std::string>(name));
}

const Buffer& DataNode::GetBuffer() const {
	return m_buffer;
}
