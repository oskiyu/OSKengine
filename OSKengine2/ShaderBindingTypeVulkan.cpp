#include "ShaderBindingTypeVulkan.h"

#include "ShaderBindingType.h"
#include "EnumFlags.hpp"

VkDescriptorType OSK::GetDescriptorTypeVk(ShaderBindingType type) {
	switch (type) {
	case OSK::ShaderBindingType::UNIFORM_BUFFER:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	case OSK::ShaderBindingType::TEXTURE:
		return VK_DESCRIPTOR_TYPE_SAMPLER;
	default:
		return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}
}

VkShaderStageFlags OSK::GetShaderStageVk(ShaderStage stage) {
	VkShaderStageFlags output = 0;

	if (EFTraits::HasFlag(stage, ShaderStage::VERTEX))
		output |= VK_SHADER_STAGE_VERTEX_BIT;

	if (EFTraits::HasFlag(stage, ShaderStage::FRAGMENT))
		output |= VK_SHADER_STAGE_FRAGMENT_BIT;

	return output;
}
