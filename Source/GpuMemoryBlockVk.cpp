#include "GpuMemoryBlockVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "GpuMemoryTypeVk.h"
#include "GpuMemoryTypes.h"
#include "GpuVk.h"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include "FormatVk.h"
#include "Format.h"
#include "IGpuMemorySubblock.h"
#include "GpuMemorySubblockVk.h"
#include "GpuImageVk.h"
#include "OSKengine.h"
#include "IRenderer.h"

#include "GpuMemoryExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


GpuMemoryBlockVk::~GpuMemoryBlockVk() {
	// Eliminamos todos los subbloques.
	subblocks.Free();

	if (buffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(device->As<GpuVk>()->GetLogicalDevice(), buffer, nullptr);
		buffer = VK_NULL_HANDLE;
	}

	if (memory != VK_NULL_HANDLE) {
		vkFreeMemory(device->As<GpuVk>()->GetLogicalDevice(), memory, nullptr);
		memory = VK_NULL_HANDLE;
	}
}

OwnedPtr<GpuMemoryBlockVk> GpuMemoryBlockVk::CreateNewBufferBlock(USize64 reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage) {
	return new GpuMemoryBlockVk(reservedSize, device, type, bufferUSage);
}
OwnedPtr<GpuMemoryBlockVk> GpuMemoryBlockVk::CreateNewImageBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage) {
	return new GpuMemoryBlockVk(image, device, imageUSage, type);
}

GpuMemoryBlockVk::GpuMemoryBlockVk(USize64 reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage)
	: IGpuMemoryBlock(reservedSize, device, type, GpuMemoryUsage::BUFFER) {

	const VkDevice logicalDevice = device->As<GpuVk>()->GetLogicalDevice();

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = reservedSize;
	bufferInfo.usage = GetGpuBufferUsageVk(bufferUSage);
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer);
	OSK_ASSERT(result == VK_SUCCESS, GpuBufferCreationException(result));
		
	VkMemoryRequirements memRequirements{};
	vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

	// Flags de propiedades de la asignación de memoria.
	VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo{};
	memoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
	memoryAllocateFlagsInfo.flags = GetMemoryAllocateFlags(bufferUSage);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = GetMemoryType(memRequirements.memoryTypeBits, device->As<GpuVk>(), type);
	allocInfo.pNext = &memoryAllocateFlagsInfo;

	result = vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &memory);
	OSK_ASSERT(result == VK_SUCCESS, GpuMemoryAllocException(result));

	vkBindBufferMemory(logicalDevice, buffer, memory, 0);
}

OwnedPtr<IGpuMemorySubblock> GpuMemoryBlockVk::CreateNewMemorySubblock(USize64 size, USize64 offset) {
	return new GpuMemorySubblockVk(this, size, offset);
}

GpuMemoryBlockVk::GpuMemoryBlockVk(GpuImage* image, IGpu* device, GpuImageUsage imageUsage, GpuSharedMemoryType type)
	: IGpuMemoryBlock(
		GetFormatNumberOfBytes(image->GetFormat()) * image->GetSize2D().x * image->GetSize2D().y, 
		device, type, GpuMemoryUsage::IMAGE) {
	
	VkMemoryRequirements memRequirements{};
	vkGetImageMemoryRequirements(device->As<GpuVk>()->GetLogicalDevice(), image->As<GpuImageVk>()->GetVkImage(), &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = GetMemoryType(memRequirements.memoryTypeBits, device->As<GpuVk>(), type);

	VkResult result = vkAllocateMemory(device->As<GpuVk>()->GetLogicalDevice(), &allocInfo, nullptr, &memory);
	OSK_ASSERT(result == VK_SUCCESS, GpuMemoryAllocException(result));

	vkBindImageMemory(device->As<GpuVk>()->GetLogicalDevice(), image->As<GpuImageVk>()->GetVkImage(), memory, 0);
}

VkDeviceMemory GpuMemoryBlockVk::GetVulkanMemory() const {
	return memory;
}

VkBuffer GpuMemoryBlockVk::GetVulkanBuffer() const {
	return buffer;
}

VkMemoryAllocateFlags GpuMemoryBlockVk::GetMemoryAllocateFlags(GpuBufferUsage usage) {
	VkMemoryAllocateFlags output = 0;

	if (EFTraits::HasFlag(usage, GpuBufferUsage::RT_ACCELERATION_STRUCTURE))
		output |= VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

	if (EFTraits::HasFlag(usage, GpuBufferUsage::RT_ACCELERATION_STRUCTURE_BUILDING))
		output |= VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

	if (EFTraits::HasFlag(usage, GpuBufferUsage::RT_SHADER_BINDING_TABLE))
		output |= VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

	if (Engine::GetRenderer()->SupportsRaytracing() &&
		(EFTraits::HasFlag(usage, GpuBufferUsage::VERTEX_BUFFER) || EFTraits::HasFlag(usage, GpuBufferUsage::INDEX_BUFFER)))
		output |= VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

	return output;
}

uint32_t GpuMemoryBlockVk::GetMemoryType(uint32_t memoryTypeFilter, GpuVk* device, GpuSharedMemoryType type) {
	auto nativeSharedMode = GetGpuSharedMemoryTypeVk(type);

	for (uint32_t i = 0; i < device->GetInfo().memoryProperties.memoryTypeCount; i++)
		if ((memoryTypeFilter & (1 << i)) && ((device->GetInfo().memoryProperties.memoryTypes[i].propertyFlags & nativeSharedMode) == nativeSharedMode))
			return i;

	OSK_ASSERT(false, GpuMemoryAllocException(0));

	return -1;
}

#endif
