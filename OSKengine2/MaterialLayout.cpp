#include "MaterialLayout.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

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
