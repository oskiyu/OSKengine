#include "ShaderBindingType.h"

#include "OSKmacros.h"

template <> std::string OSK::ToString<OSK::GRAPHICS::ShaderBindingType>(const OSK::GRAPHICS::ShaderBindingType& type) {
	USize32 numFlags = 0;

	std::string output = "";

	if (type == OSK::GRAPHICS::ShaderBindingType::UNIFORM_BUFFER) {
		if (numFlags != 0)
			output += " | ";
		output += "ShaderBindingType::UNIFORM_BUFFER";

		numFlags++;
	}

	if (type == OSK::GRAPHICS::ShaderBindingType::TEXTURE) {
		if (numFlags != 0)
			output += " | ";
		output += "ShaderBindingType::TEXTURE";

		numFlags++;
	}

	if (type == OSK::GRAPHICS::ShaderBindingType::CUBEMAP) {
		if (numFlags != 0)
			output += " | ";
		output += "ShaderBindingType::CUBEMAP";

		numFlags++;
	}

	if (type == OSK::GRAPHICS::ShaderBindingType::STORAGE_BUFFER) {
		if (numFlags != 0)
			output += " | ";
		output += "ShaderBindingType::STORAGE_BUFFER";

		numFlags++;
	}

	if (type == OSK::GRAPHICS::ShaderBindingType::RT_ACCELERATION_STRUCTURE) {
		if (numFlags != 0)
			output += " | ";
		output += "ShaderBindingType::RT_ACCELERATION_STRUCTURE";

		numFlags++;
	}

	if (type == OSK::GRAPHICS::ShaderBindingType::RT_TARGET_IMAGE) {
		if (numFlags != 0)
			output += " | ";
		output += "ShaderBindingType::RT_TARGET_IMAGE";

		numFlags++;
	}

	if (numFlags == 0)
		return "Unknown";

	return output;
}


template <> std::string OSK::ToString<OSK::GRAPHICS::ShaderStage>(const OSK::GRAPHICS::ShaderStage& stage) {
	USize32 numFlags = 0;

	std::string output = "";

	if (EFTraits::HasFlag(stage, OSK::GRAPHICS::ShaderStage::VERTEX)) {
		if (numFlags != 0)
			output += " | ";
		output += "ShaderStage::VERTEX";
	}

	if (EFTraits::HasFlag(stage, OSK::GRAPHICS::ShaderStage::FRAGMENT)) {
		if (numFlags != 0)
			output += " | ";
		output += "ShaderStage::FRAGMENT";
	}

	if (EFTraits::HasFlag(stage, OSK::GRAPHICS::ShaderStage::TESSELATION_CONTROL)) {
		if (numFlags != 0)
			output += " | ";
		output += "ShaderStage::TESSELATION_CONTROL";
	}

	if (EFTraits::HasFlag(stage, OSK::GRAPHICS::ShaderStage::TESSELATION_EVALUATION)) {
		if (numFlags != 0)
			output += " | ";
		output += "ShaderStage::TESSELATION_EVALUATION";
	}

	if (EFTraits::HasFlag(stage, OSK::GRAPHICS::ShaderStage::RT_RAYGEN)) {
		if (numFlags != 0)
			output += " | ";
		output += "ShaderStage::RT_RAYGEN";
	}

	if (EFTraits::HasFlag(stage, OSK::GRAPHICS::ShaderStage::RT_CLOSEST_HIT)) {
		if (numFlags != 0)
			output += " | ";
		output += "ShaderStage::RT_CLOSEST_HIT";
	}

	if (EFTraits::HasFlag(stage, OSK::GRAPHICS::ShaderStage::RT_MISS)) {
		if (numFlags != 0)
			output += " | ";
		output += "ShaderStage::RT_MISS";
	}

	if (EFTraits::HasFlag(stage, OSK::GRAPHICS::ShaderStage::COMPUTE)) {
		if (numFlags != 0)
			output += " | ";
		output += "ShaderStage::COMPUTE";
	}

	if (numFlags == 0)
		return "Unknown";

	return output;
}
