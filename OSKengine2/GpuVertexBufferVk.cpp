#include "GpuVertexBufferVk.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuVertexBufferVk::GpuVertexBufferVk(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment, TSize numVertices, const VertexInfo& vertexInfo)
	: IGpuVertexBuffer(buffer, size, alignment, numVertices, vertexInfo) {

}
