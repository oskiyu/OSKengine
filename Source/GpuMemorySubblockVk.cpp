#include "GpuMemorySubblockVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include <vulkan/vulkan.h>
#include "GpuMemoryBlockVk.h"
#include "GpuVk.h"
#include "Memory.h"

#include "Assert.h"
#include "GpuMemoryExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

template <VulkanTarget Target>
GpuMemorySubblockVk<Target>::GpuMemorySubblockVk(IGpuMemoryBlock* owner, UIndex64 size, UIndex64 offset)
	: IGpuMemorySubblock(owner, size, offset) {

}

#endif
