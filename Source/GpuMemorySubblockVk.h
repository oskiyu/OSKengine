#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "IGpuMemorySubblock.h"
#include "VulkanTarget.h"

namespace OSK::GRAPHICS {

	template <VulkanTarget Target>
	class OSKAPI_CALL GpuMemorySubblockVk : public IGpuMemorySubblock {

	public:

		GpuMemorySubblockVk(IGpuMemoryBlock* owner, UIndex64 size, UIndex64 offset);

	};

	template class GpuMemorySubblockVk<VulkanTarget::VK_1_0>;
	template class GpuMemorySubblockVk<VulkanTarget::VK_LATEST>;

}

#endif
