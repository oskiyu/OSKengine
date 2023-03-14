#include "GpuUniformBufferVk.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuUniformBufferVk::GpuUniformBufferVk(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: IGpuUniformBuffer(buffer, size, alignment) {

}
