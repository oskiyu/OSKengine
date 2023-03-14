#include "GpuMemorySubblockVk.h"

#include <vulkan/vulkan.h>
#include "GpuMemoryBlockVk.h"
#include "GpuVk.h"
#include "Memory.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuMemorySubblockVk::GpuMemorySubblockVk(IGpuMemoryBlock* owner, TSize size, TSize offset)
	: IGpuMemorySubblock(owner, size, offset) {

}

void GpuMemorySubblockVk::MapMemory() {
	MapMemory(reservedSize, 0);
}

void GpuMemorySubblockVk::MapMemory(TSize size, TSize offset) {
	vkMapMemory(ownerBlock->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		ownerBlock->As<GpuMemoryBlockVk>()->GetVulkanMemory(),
		totalOffsetFromBlock + offset,
		size,
		0,
		&mappedData);

	isMapped = true;
}

void GpuMemorySubblockVk::Write(const void* data, TSize size) {
	WriteOffset(data, size, cursor);
	cursor += size;
}

void GpuMemorySubblockVk::WriteOffset(const void* data, TSize size, TSize offset) {
	OSK_ASSERT(isMapped, "El buffer no está mapeado.");

	memcpy((char*)mappedData + offset, data, size);
}

void GpuMemorySubblockVk::Unmap() {
	if (!isMapped)
		return;

	vkUnmapMemory(ownerBlock->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		ownerBlock->As<GpuMemoryBlockVk>()->GetVulkanMemory());

	isMapped = false;
	mappedData = nullptr;
	cursor = 0;
}
