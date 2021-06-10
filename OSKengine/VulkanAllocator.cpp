#include "VulkanAllocator.h"

#include "GPUDataBuffer.h"

using namespace OSK;
using namespace OSK::VULKAN;

void MemoryAllocator::Init(VkDevice device, VkPhysicalDeviceMemoryProperties gpuMemoryProperties) {
	logicalDevice = device;
	this->gpuMemoryProperties = gpuMemoryProperties;
}

void MemoryAllocator::Allocate(GpuDataBuffer* buffer, size_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
	OSK_ASSERT(logicalDevice != VK_NULL_HANDLE, "No se ha inicializado el asignador de memoria de la GPU.");
	
	for (auto& block : blocks) {
		if (block->usage != usage || block->memoryProperties != properties)
			continue;

		if (block->AllocateBuffer(buffer, size))
			return;
	}

	size_t newBlockSize = SIZE_OF_GPU_MEMORY_BLOCK;
	if (size > SIZE_OF_GPU_MEMORY_BLOCK)
		newBlockSize = size;

	MemoryBlock* block = new MemoryBlock(logicalDevice, gpuMemoryProperties, &stats);
	block->InitialAlloc(newBlockSize, properties, usage);
	block->AllocateBuffer(buffer, size);

	stats.totalReservedSize += newBlockSize;
	stats.totalMemoryAllocations++;

	blocks.emplace_back(block);
}

void MemoryAllocator::Free() {
	for (auto& block : blocks)
		block->Free();

	blocks.clear();
}
