#include "MaterialLayout.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


MaterialLayout::MaterialLayout(const std::string& materialName) : m_name(materialName) {

}

void MaterialLayout::AddSlot(const MaterialLayoutSlot& slot) {
	m_slots[slot.name] = slot;
}

void MaterialLayout::AddPushConstant(MaterialLayoutPushConstant pushConstant) {
	pushConstant.offset = m_accumulatedPushConstOffset;
	m_pushConstants[pushConstant.name] = pushConstant;

	m_accumulatedPushConstOffset += pushConstant.size;
}

const MaterialLayoutSlot& MaterialLayout::GetSlot(std::string_view name) const {
	return m_slots.find(name)->second;
}

const MaterialLayoutPushConstant& MaterialLayout::GetPushConstant(std::string_view name) const {
	return m_pushConstants.at(static_cast<std::string>(name));
}

MaterialLayoutSlot& MaterialLayout::GetSlot(std::string_view name) {
	return m_slots.find(name)->second;
}

MaterialLayoutPushConstant& MaterialLayout::GetPushConstant(std::string_view name) {
	return m_pushConstants.at(static_cast<std::string>(name));
}

const MaterialLayout::MaterialSlotsIterable& MaterialLayout::GetAllSlots() const {
	return m_slots;
}

const MaterialLayout::PushConstantsIterable& MaterialLayout::GetAllPushConstants() const {
	return m_pushConstants;
}

MaterialLayout::MaterialSlotsIterable& MaterialLayout::GetAllSlots() {
	return m_slots;
}

MaterialLayout::PushConstantsIterable& MaterialLayout::GetAllPushConstants() {
	return m_pushConstants;
}

std::string_view MaterialLayout::GetMaterialName() const {
	return m_name;
}


template <> std::string OSK::ToString<OSK::GRAPHICS::MaterialLayout>(const OSK::GRAPHICS::MaterialLayout& layout) {
	std::string output = std::format("Material layout for {}.\n", layout.GetMaterialName());

	output += "Shader slots: \n";

	for (const auto& [_name, slot] : layout.GetAllSlots()) {
		output += ToString(slot);
	}

	output += "\nShader push constants: \n";

	for (const auto& [_name, pConst] : layout.GetAllPushConstants()) {
		output += ToString(pConst);
	}

	return output;
}

template <> std::string OSK::ToString<OSK::GRAPHICS::MaterialLayoutSlot>(const OSK::GRAPHICS::MaterialLayoutSlot& slot) {
	std::string output = std::format("\tSlot index: {}\n", slot.glslSetIndex);

	output += std::format("\tSlot name: {}\n", slot.name);
	output += std::format("\tSlot stage: {}\n", ToString(slot.stage));

	output += "\tSlot bindings: \n";

	for (const auto& [_name, binding] : slot.bindings) {
		output += std::format("{}\n", ToString(binding));
	}

	return output;
}

template <> std::string OSK::ToString<OSK::GRAPHICS::MaterialLayoutBinding>(const OSK::GRAPHICS::MaterialLayoutBinding& binding) {
	std::string output = std::format("\t\tIndex: {}\n", binding.glslIndex);
	output += std::format("\t\tName: {}\n", binding.name);
	output += std::format("\t\tType: {}\n", ToString(binding.type));
	output += std::format("\t\tIs bindless array: {}\n", binding.isUnsizedArray);
	output += std::format("\t\tNum. array layers: {}\n", binding.numArrayLayers);

	return output;
}

template <> std::string OSK::ToString<OSK::GRAPHICS::MaterialLayoutPushConstant>(const OSK::GRAPHICS::MaterialLayoutPushConstant& pConst) {
	std::string output = std::format("\tName: \n", pConst.name);

	output += std::format("\tShader stage: {}\n", ToString(pConst.stage));
	output += std::format("\tSize (in bytes): {}\n", pConst.size);
	output += std::format("\tOffset (in bytes): {}\n", pConst.offset);

	return output;
}
