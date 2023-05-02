#include "GpuMemoryTypeVk.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuMemoryTypes.h"
#include "GpuImageUsage.h"
#include <vulkan/vulkan.h>
#include "ToString.h"

#define OSK_CONTAINS_FLAG(a, b) EFTraits::HasFlag(a, b)

unsigned int OSK::GRAPHICS::GetGpuSharedMemoryTypeVk(GpuSharedMemoryType type) {
	switch (type) {

	case GpuSharedMemoryType::GPU_ONLY:
		return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	case GpuSharedMemoryType::GPU_AND_CPU:
		return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;

	}

	return 0;
}

unsigned int OSK::GRAPHICS::GetGpuBufferUsageVk(GpuBufferUsage usage) {
	unsigned int flags = 0;

	if (OSK_CONTAINS_FLAG(usage, GpuBufferUsage::UNIFORM_BUFFER))
		flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuBufferUsage::VERTEX_BUFFER)) {
		flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

		if (Engine::GetRenderer()->SupportsRaytracing())
			flags |= 
				VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | 
				VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | 
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	}

	if (OSK_CONTAINS_FLAG(usage, GpuBufferUsage::STORAGE_BUFFER))
		flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuBufferUsage::INDEX_BUFFER)) {
		flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

		if (Engine::GetRenderer()->SupportsRaytracing())
			flags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | 
					VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | 
					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	}

	if (OSK_CONTAINS_FLAG(usage, GpuBufferUsage::TRANSFER_SOURCE))
		flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuBufferUsage::TRANSFER_DESTINATION))
		flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	if (EFTraits::HasFlag(usage, GpuBufferUsage::RT_ACCELERATION_STRUCTURE))
		flags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

	if (EFTraits::HasFlag(usage, GpuBufferUsage::RT_ACCELERATION_STRUCTURE_BUILDING))
		flags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
		| VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

	if (EFTraits::HasFlag(usage, GpuBufferUsage::RT_SHADER_BINDING_TABLE))
		flags |= 
			VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | 
			VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | 
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | 
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

	OSK_ASSERT(flags != 0, "Flags indebidas. " + ToString<GpuBufferUsage>(usage));

	return flags;
}

unsigned int OSK::GRAPHICS::GetGpuImageUsageVk(GpuImageUsage usage) {
	unsigned int flags = 0;

	if (EFTraits::HasFlag(usage, GpuImageUsage::COLOR))
		flags = flags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuImageUsage::DEPTH))
		flags = flags | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuImageUsage::STENCIL))
		flags = flags | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuImageUsage::SAMPLED))
		flags = flags | VK_IMAGE_USAGE_SAMPLED_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuImageUsage::SAMPLED_ARRAY))
		flags = flags | VK_IMAGE_USAGE_SAMPLED_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuImageUsage::TRANSFER_SOURCE))
		flags = flags | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuImageUsage::TRANSFER_DESTINATION))
		flags = flags | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuImageUsage::RT_TARGET_IMAGE))
		flags = flags | VK_IMAGE_USAGE_STORAGE_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuImageUsage::COMPUTE))
		flags = flags | VK_IMAGE_USAGE_STORAGE_BIT;

	return flags;
}


unsigned int OSK::GRAPHICS::GetGpuImageAspectVk(GpuImageUsage usage) {
	unsigned int flags = 0;

	if (EFTraits::HasFlag(usage, GpuImageUsage::COLOR) || 
		EFTraits::HasFlag(usage, GpuImageUsage::RT_TARGET_IMAGE))
		flags = flags | VK_IMAGE_ASPECT_COLOR_BIT;

	if (EFTraits::HasFlag(usage, GpuImageUsage::SAMPLED) && 
		!EFTraits::HasFlag(usage, GpuImageUsage::DEPTH) && 
		!EFTraits::HasFlag(usage, GpuImageUsage::STENCIL))
		flags = flags | VK_IMAGE_ASPECT_COLOR_BIT;

	if (OSK_CONTAINS_FLAG(usage, GpuImageUsage::DEPTH))
		flags = flags | (VK_IMAGE_ASPECT_DEPTH_BIT);

	if (OSK_CONTAINS_FLAG(usage, GpuImageUsage::STENCIL))
		flags = flags | (VK_IMAGE_ASPECT_STENCIL_BIT);

	return flags;
}
