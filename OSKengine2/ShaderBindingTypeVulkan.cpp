#include "ShaderBindingTypeVulkan.h"

#include "ShaderBindingType.h"
#include "EnumFlags.hpp"

VkDescriptorType OSK::GRAPHICS::GetDescriptorTypeVk(ShaderBindingType type) {
	switch (type) {
	case ShaderBindingType::UNIFORM_BUFFER:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	case ShaderBindingType::TEXTURE:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case ShaderBindingType::TEXTURE_ARRAY:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case ShaderBindingType::CUBEMAP:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case ShaderBindingType::STORAGE_BUFFER:
		return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	case ShaderBindingType::RT_ACCELERATION_STRUCTURE:
		return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	case ShaderBindingType::RT_TARGET_IMAGE:
		return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
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

	if (EFTraits::HasFlag(stage, ShaderStage::TESSELATION_CONTROL))
		output |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

	if (EFTraits::HasFlag(stage, ShaderStage::TESSELATION_EVALUATION))
		output |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

	if (EFTraits::HasFlag(stage, ShaderStage::RT_RAYGEN))
		output |= VK_SHADER_STAGE_RAYGEN_BIT_KHR;

	if (EFTraits::HasFlag(stage, ShaderStage::RT_CLOSEST_HIT))
		output |= VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

	if (EFTraits::HasFlag(stage, ShaderStage::RT_MISS))
		output |= VK_SHADER_STAGE_MISS_BIT_KHR;

	if (EFTraits::HasFlag(stage, ShaderStage::COMPUTE))
		output |= VK_SHADER_STAGE_COMPUTE_BIT;

	return output;
}
