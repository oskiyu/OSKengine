#include "GPUDataBuffer.h"

#include "Memory.h"

using namespace OSK;
using namespace OSK::Memory;

void GPUDataBuffer::Create(VkDevice device, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkPhysicalDeviceMemoryProperties gpuProps) {
	logicalDevice = device;
	this->usage = usage;
	this->properties = properties;
	this->gpuProps = gpuProps;
}

void GPUDataBuffer::Allocate(size_t size) {
	this->size = size;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer);
	if (result != VK_SUCCESS) {
		Logger::Log(LogMessageLevels::BAD_ERROR, std::string(__FUNCTION__) + " crear buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = getMemoryType(memRequirements.memoryTypeBits);
	if (allocInfo.memoryTypeIndex < 0) {
		Logger::Log(LogMessageLevels::BAD_ERROR, std::string(__FUNCTION__) + " no se encuentra memoria compatible");
	}

	result = vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &memory);
	if (result != VK_SUCCESS) {
		Logger::Log(LogMessageLevels::BAD_ERROR, std::string(__FUNCTION__) + " alloc buffer memory.");
	}

	vkBindBufferMemory(logicalDevice, buffer, memory, 0);
}

void GPUDataBuffer::MapMemory() {
	MapMemory(size, 0);
}

void GPUDataBuffer::MapMemory(size_t size, size_t offset) {
	vkMapMemory(logicalDevice, memory, offset, size, 0, &mappedData);
	isMapped = true;
}

void GPUDataBuffer::Write(const void* data, size_t size, size_t offset) {
	if (!isMapped)
		MapMemory(size, offset);

	WriteMapped(data, size, 0);

	UnmapMemory();
}

void GPUDataBuffer::WriteMapped(const void* data, size_t size, size_t offset) {
	void* target = mappedData;
	if (offset)
		target = AddPtrOffset(mappedData, offset);

	memcpy(target, data, size);
}

void GPUDataBuffer::UnmapMemory() {
	if (isMapped)
		vkUnmapMemory(logicalDevice, memory);

	isMapped = false;
}

void GPUDataBuffer::Free() {
	if (buffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(logicalDevice, buffer, nullptr);
		buffer = VK_NULL_HANDLE;
	}
	if (memory != VK_NULL_HANDLE) {
		vkFreeMemory(logicalDevice, memory, nullptr);
		memory = VK_NULL_HANDLE;
	}
}

void GPUDataBuffer::SetDynamicUboStructureSize(size_t size) {
	dynamicSize = size;
}

size_t GPUDataBuffer::GetSize() {
	return size;
}

size_t GPUDataBuffer::GetDynamicUboStructureSize() {
	return dynamicSize;
}
	
GPUDataBuffer::~GPUDataBuffer() {
	//Free();
}
	
uint32_t GPUDataBuffer::getMemoryType(const uint32_t& memoryTypeFilter) {
	for (uint32_t i = 0; i < gpuProps.memoryTypeCount; i++)
		if (memoryTypeFilter & (1 << i) && (gpuProps.memoryTypes[i].propertyFlags & properties) == properties)
			return i;

	return -1;
}
