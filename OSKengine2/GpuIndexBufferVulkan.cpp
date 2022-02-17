#include "GpuIndexBufferVulkan.h"

using namespace OSK;

GpuIndexBufferVulkan::GpuIndexBufferVulkan(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: IGpuIndexBuffer(buffer, size, alignment) {

}
