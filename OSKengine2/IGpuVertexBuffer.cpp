#include "IGpuVertexBuffer.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IGpuVertexBuffer::IGpuVertexBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment, TSize numVertices, const VertexInfo& vertexInfo)
	: GpuDataBuffer(buffer, size, alignment), numVertices(numVertices), vertexInfo(vertexInfo) {

}

TSize IGpuVertexBuffer::GetNumVertices() const {
	return numVertices;
}

const VertexInfo& IGpuVertexBuffer::GetVertexInfo() const {
	return vertexInfo;
}
