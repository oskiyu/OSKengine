#include "GpuVertexBufferVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuVertexBufferVulkan::GpuVertexBufferVulkan(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment, TSize numVertices, const VertexInfo& vertexInfo)
	: IGpuVertexBuffer(buffer, size, alignment, numVertices, vertexInfo) {

}
