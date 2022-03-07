#include "GpuIndexBufferVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuIndexBufferVulkan::GpuIndexBufferVulkan(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: IGpuIndexBuffer(buffer, size, alignment) {

}
