#include "VulkanMemoryBlock.h"

#include "GPUDataBuffer.h"

using namespace OSK;
using namespace OSK::VULKAN;

MemoryBlock::MemoryBlock(VkDevice logicalDevice, VkPhysicalDeviceMemoryProperties gpuMemoryProperties, MemoryAllocatorStats_t* allocatorUsedMemory)
	: logicalDevice(logicalDevice), gpuMemoryProperties(gpuMemoryProperties), stats(allocatorUsedMemory) {

}

MemoryBlock::~MemoryBlock() {

}

void MemoryBlock::Free() {
	for (auto subblock : assignedSubblocks)
		delete subblock;

	assignedSubblocks.clear();

	stats->totalReservedSize -= size;
	stats->totalUsedSize -= (size - availableSize);

	if (buffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(logicalDevice, buffer, nullptr);

		buffer = VK_NULL_HANDLE;
	}

	if (memory != VK_NULL_HANDLE) {
		vkFreeMemory(logicalDevice, memory, nullptr);

		memory = VK_NULL_HANDLE;
	}
}

void MemoryBlock::InitialAlloc(size_t size, VkMemoryPropertyFlags properties, VkBufferUsageFlags usage) {
	this->size = size;
	availableSize = size;

	memoryProperties = properties;
	this->usage = usage;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer);
	OSK_ASSERT(result == VK_SUCCESS, "Error al crear buffer en la GPU.");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = GetMemoryType(memRequirements.memoryTypeBits);
		
	result = vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &memory);
	OSK_ASSERT(result == VK_SUCCESS, "Error al asignar memoria en la GPU.");

	vkBindBufferMemory(logicalDevice, buffer, memory, 0);
}

bool MemoryBlock::AllocateBuffer(GpuDataBuffer* buffer, size_t size) {
	MemorySubblock* subblock = nullptr;
	
	bool isReused = false;

	for (auto& reuse : reusableSubblocks) {
		if (size < reuse.size) {
			subblock = new MemorySubblock;

			subblock->sizeOfSubblock = reuse.size;
			reusableSubblocks.remove(reuse);

			isReused = true;

			break;
		}
	}

	if (subblock == nullptr) {
		if (size > availableSize)
			return false;

		subblock = new MemorySubblock;
	}

	if (!isReused)
		subblock->sizeOfSubblock = size;

	subblock->totalOffsetFromBlock = this->size - availableSize;
	subblock->buffer = buffer;
	subblock->vkBuffer = this->buffer;
	subblock->memory = memory;
	subblock->logicalDevice = logicalDevice;
	subblock->ownerBlock = this;

	assignedSubblocks.push_back(subblock);

	if (!isReused)
		availableSize -= subblock->sizeOfSubblock;
	stats->totalUsedSize += subblock->sizeOfSubblock;

	buffer->memorySubblock = subblock;

	stats->totalSubAllocations++;

	return true;
}

void MemoryBlock::RemoveBlock(MemorySubblock* subblock) {
	assignedSubblocks.remove(subblock);

	ReusableMemorySubblock reuse {
		subblock->sizeOfSubblock,
		subblock->totalOffsetFromBlock
	};

	reusableSubblocks.emplace_back(reuse);

	stats->totalUsedSize -= reuse.size;
}

uint32_t MemoryBlock::GetMemoryType(uint32_t memoryTypeFilter) {
	for (uint32_t i = 0; i < gpuMemoryProperties.memoryTypeCount; i++)
		if ((memoryTypeFilter & (1 << i)) && ((gpuMemoryProperties.memoryTypes[i].propertyFlags & memoryProperties) == memoryProperties))
			return i;

	OSK_ASSERT(false, "No se encontró memoria en la GPU comptible");

	return -1;
}
