#include "MaterialBinding.h"

VkDescriptorType OSK::GetVulkanBindingType(OSK::MaterialBindingType type) {
	switch (type) {
	case OSK::MaterialBindingType::DATA_BUFFER:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	case OSK::MaterialBindingType::DYNAMIC_DATA_BUFFER:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;

	case OSK::MaterialBindingType::TEXTURE:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

	}

	return VK_DESCRIPTOR_TYPE_END_RANGE;
}

VkShaderStageFlagBits OSK::GetVulkanShaderBinding(OSK::MaterialBindingShaderStage type) {
	switch (type) {
	case OSK::MaterialBindingShaderStage::VERTEX:
		return VK_SHADER_STAGE_VERTEX_BIT;

	case OSK::MaterialBindingShaderStage::FRAGMENT:
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	}

	return VK_SHADER_STAGE_ALL;
}
