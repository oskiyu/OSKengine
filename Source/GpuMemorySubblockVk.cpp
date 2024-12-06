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

GpuMemorySubblockVk::GpuMemorySubblockVk(IGpuMemoryBlock* owner, UIndex64 size, UIndex64 offset)
	: IGpuMemorySubblock(owner, size, offset) {

}

void GpuMemorySubblockVk::MapMemory() {
	MapMemory(reservedSize, 0);
}

void GpuMemorySubblockVk::MapMemory(UIndex64 size, UIndex64 offset) {
	if (isMapped) {
		return;
	}
	
	vkMapMemory(ownerBlock->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		ownerBlock->As<GpuMemoryBlockVk>()->GetVulkanMemory(),
		totalOffsetFromBlock + offset,
		size,
		0,
		&mappedData);

	isMapped = true;
}

void GpuMemorySubblockVk::Write(const void* data, UIndex64 size) {
	WriteOffset(data, size, cursor);
	cursor += size;
}

void GpuMemorySubblockVk::WriteOffset(const void* data, UIndex64 size, UIndex64 offset) {
	OSK_ASSERT(isMapped, GpuMemoryNotMappedException());

	memcpy(static_cast<char*>(mappedData) + offset, data, size);
}

void GpuMemorySubblockVk::Unmap() {
	if (!isMapped) {
		return;
	}

	vkUnmapMemory(ownerBlock->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		ownerBlock->As<GpuMemoryBlockVk>()->GetVulkanMemory());

	isMapped = false;
	mappedData = nullptr;
	cursor = 0;
}

#endif
