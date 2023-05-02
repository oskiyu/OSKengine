#include "MaterialLayout.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void MaterialLayout::AddSlot(const MaterialLayoutSlot& slot) {
	slots.Insert(slot.name, slot);
}

void MaterialLayout::AddPushConstant(MaterialLayoutPushConstant pushConstant) {
	pushConstant.offset = accumulatedPushConstOffset;
	pushConstants.Insert(pushConstant.name, pushConstant);

	accumulatedPushConstOffset += pushConstant.size;
}

MaterialLayoutSlot& MaterialLayout::GetSlot(std::string_view name) const {
	return slots.Get(static_cast<std::string>(name));
}

MaterialLayoutPushConstant& MaterialLayout::GetPushConstant(std::string_view name) const {
	return pushConstants.Get(static_cast<std::string>(name));
}

const MaterialLayout::MaterialSlotsIterable& MaterialLayout::GetAllSlots() const {
	return slots;
}

const MaterialLayout::PushConstantsIterable& MaterialLayout::GetAllPushConstants() const {
	return pushConstants;
}

MaterialLayout::MaterialSlotsIterable& MaterialLayout::GetAllSlots() {
	return slots;
}

MaterialLayout::PushConstantsIterable& MaterialLayout::GetAllPushConstants() {
	return pushConstants;
}
