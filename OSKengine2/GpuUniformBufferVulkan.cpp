#include "GpuUniformBufferVulkan.h"

using namespace OSK;

GpuUniformBufferVulkan::GpuUniformBufferVulkan(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: IGpuUniformBuffer(buffer, size, alignment) {

}
