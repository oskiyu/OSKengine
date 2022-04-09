#include "ShaderBindingTypeVulkan.h"

#include "ShaderBindingType.h"
#include "EnumFlags.hpp"

VkDescriptorType OSK::GRAPHICS::GetDescriptorTypeVk(ShaderBindingType type) {
	switch (type) {
	case ShaderBindingType::UNIFORM_BUFFER:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	case ShaderBindingType::TEXTURE:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case ShaderBindingType::CUBEMAP:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	default:
		return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}
}

VkShaderStageFlags OSK::GRAPHICS::GetShaderStageVk(ShaderStage stage) {
	VkShaderStageFlags output = 0;

	if (EFTraits::HasFlag(stage, ShaderStage::VERTEX))
		output |= VK_SHADER_STAGE_VERTEX_BIT;

	if (EFTraits::HasFlag(stage, ShaderStage::FRAGMENT))
		output |= VK_SHADER_STAGE_FRAGMENT_BIT;

	return output;
}
