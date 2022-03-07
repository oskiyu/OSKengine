#pragma once

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	enum class ShaderBindingType;
	enum class ShaderStage;

	VkDescriptorType GetDescriptorTypeVk(ShaderBindingType type);

	VkShaderStageFlags GetShaderStageVk(ShaderStage stage);

}
