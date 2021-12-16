#include "GpuMemorySubblockVulkan.h"

#include <vulkan/vulkan.h>
#include "GpuMemoryBlockVulkan.h"
#include "GpuVulkan.h"
#include "Memory.h"

using namespace OSK;

GpuMemorySubblockVulkan::GpuMemorySubblockVulkan(IGpuMemoryBlock* owner, TSize size, TSize offset)
	: IGpuMemorySubblock(owner, size, offset) {

}

void GpuMemorySubblockVulkan::MapMemory() {
	MapMemory(reservedSize, 0);
}

void GpuMemorySubblockVulkan::MapMemory(TSize size, TSize offset) {
	vkMapMemory(ownerBlock->GetDevice()->As<GpuVulkan>()->GetLogicalDevice(),
		ownerBlock->As<GpuMemoryBlockVulkan>()->GetVulkanMemory(),
		totalOffsetFromBlock + offset,
		size,
		0,
		&mappedData);

	isMapped = true;
}

void GpuMemorySubblockVulkan::Write(const void* data, TSize size) {
	WriteOffset(data, size, cursor);
	cursor += size;
}

void GpuMemorySubblockVulkan::WriteOffset(const void* data, TSize size, TSize offset) {
	OSK_ASSERT(isMapped, "El buffer no está mapeado.");

	memcpy((char*)mappedData + offset, data, size);
}

void GpuMemorySubblockVulkan::Unmap() {
	if (!isMapped)
		return;

	vkUnmapMemory(ownerBlock->GetDevice()->As<GpuVulkan>()->GetLogicalDevice(),
		ownerBlock->As<GpuMemoryBlockVulkan>()->GetVulkanMemory());

	isMapped = false;
	mappedData = nullptr;
	cursor = 0;
}
