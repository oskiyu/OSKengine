#include "GpuUniformBufferVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuUniformBufferVulkan::GpuUniformBufferVulkan(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: IGpuUniformBuffer(buffer, size, alignment) {

}
