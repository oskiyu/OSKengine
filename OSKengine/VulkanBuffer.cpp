#include "VulkanBuffer.h"

using namespace OSK;

void VulkanBuffer::Create(VkDevice device, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkPhysicalDeviceMemoryProperties gpuProps) {
	logicalDevice = device;
	this->usage = usage;
	this->properties = properties;
	this->gpuProps = gpuProps;
}

void VulkanBuffer::Allocate(size_t size) {
	this->Size = size;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &Buffer);
	if (result != VK_SUCCESS) {
		OSK_SHOW_ERROR("crear buffer.");
		//return OskResult::ERROR_CREATE_BUFFER;
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(logicalDevice, Buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = getMemoryType(memRequirements.memoryTypeBits);
	if (allocInfo.memoryTypeIndex < 0) {
		OSK_SHOW_ERROR("no se encuentra memoria compatible.");
		//return OskResult::ERROR_GPU_BUFFER_MEMORY_COMPATIBILITY;
	}

	result = vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &Memory);
	if (result != VK_SUCCESS) {
		OSK_SHOW_ERROR("alloc buffer memory.");
		//return OskResult::ERROR_ALLOC_BUFFER_MEMORY;
	}

	vkBindBufferMemory(logicalDevice, Buffer, Memory, 0);
}

void VulkanBuffer::MapMemory() {
	MapMemory(Size, 0);
}

void VulkanBuffer::MapMemory(size_t size, size_t offset) {
	vkMapMemory(logicalDevice, Memory, offset, size, 0, &mappedData);
	isMapped = true;
}

void VulkanBuffer::Write(const void* data, size_t size, size_t offset) {
	if (!isMapped)
		MapMemory(size, offset);

	WriteMapped(data, size, 0);

	UnmapMemory();
}

void VulkanBuffer::WriteMapped(const void* data, size_t size, size_t offset) {
	void* target = mappedData;
	if (offset)
		target = osk_add_ptr_offset(mappedData, offset);

	memcpy(target, data, size);
}

void VulkanBuffer::UnmapMemory() {
	if (isMapped)
		vkUnmapMemory(logicalDevice, Memory);

	isMapped = false;
}

void VulkanBuffer::Free() {
	if (Buffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(logicalDevice, Buffer, nullptr);
		Buffer = VK_NULL_HANDLE;
	}
	if (Memory != VK_NULL_HANDLE) {
		vkFreeMemory(logicalDevice, Memory, nullptr);
		Memory = VK_NULL_HANDLE;
	}
}
	
VulkanBuffer::~VulkanBuffer() {
	//Free();
}
	
uint32_t VulkanBuffer::getMemoryType(const uint32_t& memoryTypeFilter) {
	for (uint32_t i = 0; i < gpuProps.memoryTypeCount; i++)
		if (memoryTypeFilter & (1 << i) && (gpuProps.memoryTypes[i].propertyFlags & properties) == properties)
			return i;

	return -1;
}
