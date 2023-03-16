#include "Data.h"

#include "Assert.h"

using namespace OSK;
using namespace OSK::PERSISTENCE;

DataNode::DataNode() : IDataElement(DataType::UNKNOWN) {

}

const IDataElement& DataNode::GetField(const std::string& key) const {
	OSK_ASSERT(HasElement(key), "No existe el dato " + key + ".");
	return fields.Get(key).GetValue();
}

bool DataNode::HasElement(const std::string& name) const {
	return fields.ContainsKey(name);
}
