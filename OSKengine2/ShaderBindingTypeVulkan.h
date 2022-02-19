#pragma once

#include <vulkan/vulkan.h>

namespace OSK {

	enum class ShaderBindingType;
	enum class ShaderStage;

	VkDescriptorType GetDescriptorTypeVk(ShaderBindingType type);

	VkShaderStageFlags GetShaderStageVk(ShaderStage stage);

}
