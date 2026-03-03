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


template <VulkanTarget Target>
GpuMemoryBlockVk<Target>::~GpuMemoryBlockVk() {
	// Eliminamos todos los subbloques.
	subblocks.Free();
	const auto logicalDevice = device->As<GpuVk<Target>>()->GetLogicalDevice();

	if (buffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(logicalDevice, buffer, nullptr);
		buffer = VK_NULL_HANDLE;
	}

	if (memory != VK_NULL_HANDLE) {
		vkFreeMemory(logicalDevice, memory, nullptr);
		memory = VK_NULL_HANDLE;
	}
}

template <VulkanTarget Target>
UniquePtr<GpuMemoryBlockVk<Target>> GpuMemoryBlockVk<Target>::CreateNewBufferBlock(USize64 reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage) {
	return UniquePtr<GpuMemoryBlockVk>(new GpuMemoryBlockVk(reservedSize, device, type, bufferUSage));
}

template <VulkanTarget Target>
UniquePtr<GpuMemoryBlockVk<Target>> GpuMemoryBlockVk<Target>::CreateNewImageBlock(GpuImage* image, IGpu* device, GpuSharedMemoryType type, GpuImageUsage imageUSage) {
	return UniquePtr<GpuMemoryBlockVk>(new GpuMemoryBlockVk(image, device, imageUSage, type));
}

template <VulkanTarget Target>
GpuMemoryBlockVk<Target>::GpuMemoryBlockVk(USize64 reservedSize, IGpu* device, GpuSharedMemoryType type, GpuBufferUsage bufferUSage)
	: IGpuMemoryBlock(reservedSize, device, type, GpuMemoryUsage::BUFFER) {

	const VkDevice logicalDevice = device->As<GpuVk<Target>>()->GetLogicalDevice();

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
	allocInfo.memoryTypeIndex = GetMemoryType(memRequirements.memoryTypeBits, device->As<GpuVk<Target>>(), type);
	allocInfo.pNext = &memoryAllocateFlagsInfo;

	result = vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &memory);
	OSK_ASSERT(result == VK_SUCCESS, GpuMemoryAllocException(result));

	vkBindBufferMemory(logicalDevice, buffer, memory, 0);
}

template <VulkanTarget Target>
UniquePtr<IGpuMemorySubblock> GpuMemoryBlockVk<Target>::CreateNewMemorySubblock(USize64 size, USize64 offset) {
	return MakeUnique<GpuMemorySubblockVk<Target>>(this, size, offset);
}

template <VulkanTarget Target>
GpuMemoryBlockVk<Target>::GpuMemoryBlockVk(GpuImage* image, IGpu* device, GpuImageUsage imageUsage, GpuSharedMemoryType type)
	: IGpuMemoryBlock(
		GetFormatNumberOfBytes(image->GetFormat()) * image->GetSize2D().x * image->GetSize2D().y, 
		device, type, GpuMemoryUsage::IMAGE) {
	
	const VkDevice logicalDevice = device->As<GpuVk<Target>>()->GetLogicalDevice();

	VkMemoryRequirements memRequirements{};
	vkGetImageMemoryRequirements(logicalDevice, image->As<GpuImageVk<Target>>()->GetVkImage(), &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = GetMemoryType(memRequirements.memoryTypeBits, device->As<GpuVk<Target>>(), type);

	VkResult result = vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &memory);
	OSK_ASSERT(result == VK_SUCCESS, GpuMemoryAllocException(result));

	vkBindImageMemory(logicalDevice, image->As<GpuImageVk<Target>>()->GetVkImage(), memory, 0);
}

template <VulkanTarget Target>
VkDeviceMemory GpuMemoryBlockVk<Target>::GetVulkanMemory() const {
	return memory;
}

template <VulkanTarget Target>
VkBuffer GpuMemoryBlockVk<Target>::GetVulkanBuffer() const {
	return buffer;
}

template <VulkanTarget Target>
VkMemoryAllocateFlags GpuMemoryBlockVk<Target>::GetMemoryAllocateFlags(GpuBufferUsage usage) {
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

template <VulkanTarget Target>
uint32_t GpuMemoryBlockVk<Target>::GetMemoryType(uint32_t memoryTypeFilter, GpuVk<Target>* device, GpuSharedMemoryType type) {
	auto nativeSharedMode = GetGpuSharedMemoryTypeVk(type);

	for (uint32_t i = 0; i < device->GetInfo().memoryProperties.memoryTypeCount; i++)
		if ((memoryTypeFilter & (1 << i)) && ((device->GetInfo().memoryProperties.memoryTypes[i].propertyFlags & nativeSharedMode) == nativeSharedMode))
			return i;

	OSK_ASSERT(false, GpuMemoryAllocException(0));

	return -1;
}

template <VulkanTarget Target>
TByte* GpuMemoryBlockVk<Target>::MapRange_Impl(USize64 offset, USize64 size) 
{
	TByte* output;

	vkMapMemory(
		GetGpu()->As<GpuVk<Target>>()->GetLogicalDevice(),
		GetVulkanMemory(),
		offset,
		size,
		0,
		(void**)&output);

	return output;
}

template <VulkanTarget Target>
void GpuMemoryBlockVk<Target>::UnmapAll_Impl() {
	vkUnmapMemory(
		GetGpu()->As<GpuVk<Target>>()->GetLogicalDevice(),
		GetVulkanMemory());
}

#endif
