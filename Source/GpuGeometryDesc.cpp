#include "GpuGeometryDesc.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuGeometryDesc::GpuGeometryDesc(const DynamicArray<TIndexSize>& indexes) 
	: m_indexes(indexes) {

}

const DynamicArray<TIndexSize>& GpuGeometryDesc::GetIndexes() const {
	return m_indexes;
}

VerticesAttributesMaps& GpuGeometryDesc::GetAttributesMap() {
	return m_attributes;
}

const VerticesAttributesMaps& GpuGeometryDesc::GetAttributesMap() const {
	return m_attributes;
}
