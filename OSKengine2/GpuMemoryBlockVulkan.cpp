#include "GpuMemoryBlockVulkan.h"

#include "GpuMemoryTypeVulkan.h"
#include "GpuMemoryTypes.h"
#include "GpuVulkan.h"
#include <vulkan/vulkan.h>
#include <glm.hpp>
#include "FormatVulkan.h"
#include "Format.h"
#include "IGpuMemorySubblock.h"
#include "GpuMemorySubblockVulkan.h"
#include "GpuImageVulkan.h"

using namespace OSK;

uint32_t GetMemoryType(uint32_t memoryTypeFilter, GpuVulkan* device, GpuSharedMemoryType type) {
	auto nativeSharedMode = GetGpuSharedMemoryTypeVulkan(type);

	for (uint32_t i = 0; i < device->GetInfo().memoryProperties.memoryTypeCount; i++)
		if ((memoryTypeFilter & (1 << i)) && ((device->GetInfo().memoryProperties.memoryTypes[i].propertyFlags & nativeSharedMode) == nativeSharedMode))
			return i;

	OSK_ASSERT(false, "No se encontró memoria en la GPU comptible");

	return -1;
}

GpuMemoryBlockVulkan::~GpuMemoryBlockVulkan() {
	Free();
}

OwnedPtr<GpuMemoryBlockVulkan> GpuMemoryBlockVulkan::CreateNewBufferBlock(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage) {
	return new GpuMemoryBlockVulkan(reservedSize, device, type, bufferUSage);
}
OwnedPtr<GpuMemoryBlockVulkan> GpuMemoryBlockVulkan::CreateNewImageBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage) {
	return new GpuMemoryBlockVulkan(image, device, imageUSage, type);
}

GpuMemoryBlockVulkan::GpuMemoryBlockVulkan(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage)
	: IGpuMemoryBlock(reservedSize, device, type, GpuMemoryUsage::BUFFER) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = reservedSize;
	bufferInfo.usage = GetGpuBufferUsageVulkan(bufferUSage);
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(device->As<GpuVulkan>()->GetLogicalDevice(), &bufferInfo, nullptr, &buffer);
	OSK_ASSERT(result == VK_SUCCESS, "Error al crear buffer en la GPU.");
		
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device->As<GpuVulkan>()->GetLogicalDevice(), buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = GetMemoryType(memRequirements.memoryTypeBits, device->As<GpuVulkan>(), type);

	result = vkAllocateMemory(device->As<GpuVulkan>()->GetLogicalDevice(), &allocInfo, nullptr, &memory);
	OSK_ASSERT(result == VK_SUCCESS, "Error al asignar memoria en la GPU.");

	vkBindBufferMemory(device->As<GpuVulkan>()->GetLogicalDevice(), buffer, memory, 0);
}

OwnedPtr<IGpuMemorySubblock> GpuMemoryBlockVulkan::CreateNewMemorySubblock(TSize size, TSize offset) {
	return new GpuMemorySubblockVulkan(this, size, offset);
}

GpuMemoryBlockVulkan::GpuMemoryBlockVulkan(GpuImage* image, IGpu* device, GpuImageUsage imageUsage, GpuSharedMemoryType type)
	: IGpuMemoryBlock(GetFormatNumberOfBytes(image->GetFormat()) * image->GetSize().X * image->GetSize().Y, device, type, GpuMemoryUsage::IMAGE) {
	
	VkMemoryRequirements memRequirements{};
	vkGetImageMemoryRequirements(device->As<GpuVulkan>()->GetLogicalDevice(), image->As<GpuImageVulkan>()->GetImage(), &memRequirements);
	this;
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = GetMemoryType(memRequirements.memoryTypeBits, device->As<GpuVulkan>(), type);

	VkResult result = vkAllocateMemory(device->As<GpuVulkan>()->GetLogicalDevice(), &allocInfo, nullptr, &memory);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo reservar memoria para la imagen");

	vkBindImageMemory(device->As<GpuVulkan>()->GetLogicalDevice(), image->As<GpuImageVulkan>()->GetImage(), memory, 0);
}

void GpuMemoryBlockVulkan::Free() {
	for (auto subblock : subblocks)
		delete subblock;

	subblocks.Free();

	if (buffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(device->As<GpuVulkan>()->GetLogicalDevice(), buffer, nullptr);

		buffer = VK_NULL_HANDLE;
	}

	if (memory != VK_NULL_HANDLE) {
		vkFreeMemory(device->As<GpuVulkan>()->GetLogicalDevice(), memory, nullptr);

		memory = VK_NULL_HANDLE;
	}

}

VkDeviceMemory GpuMemoryBlockVulkan::GetVulkanMemory() const {
	return memory;
}

VkBuffer GpuMemoryBlockVulkan::GetVulkanBuffer() const {
	return buffer;
}
