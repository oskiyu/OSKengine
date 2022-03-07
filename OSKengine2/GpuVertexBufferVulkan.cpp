#include "GpuVertexBufferVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuVertexBufferVulkan::GpuVertexBufferVulkan(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: IGpuVertexBuffer(buffer, size, alignment) {

}
