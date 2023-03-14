#include "GpuMemoryBlockVk.h"

#include "GpuMemoryTypeVk.h"
#include "GpuMemoryTypes.h"
#include "GpuVk.h"
#include <vulkan/vulkan.h>
#include <glm.hpp>
#include "FormatVk.h"
#include "Format.h"
#include "IGpuMemorySubblock.h"
#include "GpuMemorySubblockVk.h"
#include "GpuImageVk.h"
#include "OSKengine.h"
#include "IRenderer.h"

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

OwnedPtr<GpuMemoryBlockVk> GpuMemoryBlockVk::CreateNewBufferBlock(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage) {
	return new GpuMemoryBlockVk(reservedSize, device, type, bufferUSage);
}
OwnedPtr<GpuMemoryBlockVk> GpuMemoryBlockVk::CreateNewImageBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage) {
	return new GpuMemoryBlockVk(image, device, imageUSage, type);
}

GpuMemoryBlockVk::GpuMemoryBlockVk(TSize reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage)
	: IGpuMemoryBlock(reservedSize, device, type, GpuMemoryUsage::BUFFER) {

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = reservedSize;
	bufferInfo.usage = GetGpuBufferUsageVk(bufferUSage);
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(device->As<GpuVk>()->GetLogicalDevice(), &bufferInfo, nullptr, &buffer);
	OSK_ASSERT(result == VK_SUCCESS, "Error al crear buffer en la GPU.");
		
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device->As<GpuVk>()->GetLogicalDevice(), buffer, &memRequirements);

	// Flags de propiedades de la asignación de memoria.
	VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo{};
	memoryAllocateFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
	memoryAllocateFlagsInfo.flags = GetMemoryAllocateFlags(bufferUSage);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = GetMemoryType(memRequirements.memoryTypeBits, device->As<GpuVk>(), type);
	allocInfo.pNext = &memoryAllocateFlagsInfo;

	result = vkAllocateMemory(device->As<GpuVk>()->GetLogicalDevice(), &allocInfo, nullptr, &memory);
	OSK_ASSERT(result == VK_SUCCESS, "Error al asignar memoria en la GPU.");

	vkBindBufferMemory(device->As<GpuVk>()->GetLogicalDevice(), buffer, memory, 0);
}

OwnedPtr<IGpuMemorySubblock> GpuMemoryBlockVk::CreateNewMemorySubblock(TSize size, TSize offset) {
	return new GpuMemorySubblockVk(this, size, offset);
}

GpuMemoryBlockVk::GpuMemoryBlockVk(GpuImage* image, IGpu* device, GpuImageUsage imageUsage, GpuSharedMemoryType type)
	: IGpuMemoryBlock(GetFormatNumberOfBytes(image->GetFormat()) * image->GetSize().X * image->GetSize().Y, device, type, GpuMemoryUsage::IMAGE) {
	
	VkMemoryRequirements memRequirements{};
	vkGetImageMemoryRequirements(device->As<GpuVk>()->GetLogicalDevice(), image->As<GpuImageVk>()->GetVkImage(), &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = GetMemoryType(memRequirements.memoryTypeBits, device->As<GpuVk>(), type);

	VkResult result = vkAllocateMemory(device->As<GpuVk>()->GetLogicalDevice(), &allocInfo, nullptr, &memory);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo reservar memoria para la imagen");

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

	OSK_ASSERT(false, "No se encontró memoria en la GPU comptible");

	return -1;
}
