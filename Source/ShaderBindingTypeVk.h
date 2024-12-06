#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	enum class ShaderBindingType;
	enum class ShaderStage;

	VkDescriptorType GetDescriptorTypeVk(ShaderBindingType type);

	VkShaderStageFlags GetShaderStageVk(ShaderStage stage);

}

#endif
