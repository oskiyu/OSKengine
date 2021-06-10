#include "VulkanMemorySubblock.h"

#include "GPUDataBuffer.h"
#include "Memory.h"
#include "VulkanMemoryBlock.h"

using namespace OSK;
using namespace OSK::Memory;
using namespace OSK::VULKAN;

MemorySubblock::~MemorySubblock() {
	Free();
}

void MemorySubblock::MapMemory() {
	MapMemory(sizeOfSubblock, 0);
}

void MemorySubblock::MapMemory(size_t size, size_t offset) {
	vkMapMemory(logicalDevice, memory, totalOffsetFromBlock + offset, size, 0, &mappedData);
	
	isMapped = true;
}

void MemorySubblock::Write(const void* data, size_t size, size_t offset) {
	if (!isMapped)
		MapMemory(size, offset);

	WriteMapped(data, size, offset);

	UnmapMemory();
}

void MemorySubblock::WriteMapped(const void* data, size_t size, size_t offset) {
	OSK_ASSERT(isMapped, "Se ha intentado escribir en un buffer que no está mapeado.");

	void* target = mappedData;
	if (offset)
		target = AddPtrOffset(mappedData, offset);

	memcpy(target, data, size);
}

void MemorySubblock::UnmapMemory() {
	OSK_CHECK(isMapped, "Se ha intentado desmapear un subbloque que ya estaba desmapeado.");

	if (isMapped)
		vkUnmapMemory(logicalDevice, memory);

	isMapped = false;
}

void MemorySubblock::Free() {
	if (buffer) {
		buffer->memorySubblock = nullptr;
		buffer = nullptr;
	}

	if (ownerBlock) {
		ownerBlock->RemoveBlock(this);
		ownerBlock = nullptr;
	}

	vkBuffer = nullptr;
	memory = nullptr;
}

size_t MemorySubblock::GetSize() const {
	return sizeOfSubblock;
}

VkDeviceMemory MemorySubblock::GetNativeGpuMemory() const {
	return memory;
}

VkBuffer MemorySubblock::GetNativeGpuBuffer() const {
	return vkBuffer;
}

size_t MemorySubblock::GetOffset() const {
	return totalOffsetFromBlock;
}
