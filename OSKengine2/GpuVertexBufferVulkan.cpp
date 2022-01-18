#include "GpuVertexBufferVulkan.h"

using namespace OSK;

GpuVertexBufferVulkan::GpuVertexBufferVulkan(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: IGpuVertexBuffer(buffer, size, alignment) {

}
