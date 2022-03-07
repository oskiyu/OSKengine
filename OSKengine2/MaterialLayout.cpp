#include "MaterialLayout.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void MaterialLayout::AddSlot(const MaterialLayoutSlot& slot) {
	slots.Insert(slot.name, slot);
	slotNames.Push(slot.name);
}

void MaterialLayout::AddPushConstant(MaterialLayoutPushConstant pushConstant) {
	static TSize accumulatedOffset = 0;

	pushConstant.offset = accumulatedOffset;
	pushConstants.Insert(pushConstant.name, pushConstant);

	accumulatedOffset += pushConstant.size;
}

MaterialLayoutSlot& MaterialLayout::GetSlot(const std::string& name) const {
	return slots.Get(name);
}

MaterialLayoutPushConstant& MaterialLayout::GetPushConstant(const std::string& name) const {
	return pushConstants.Get(name);
}

const LinkedList<std::string>& MaterialLayout::GetAllSlotNames() const {
	return slotNames;
}

const HashMap<std::string, MaterialLayoutPushConstant>& MaterialLayout::GetAllPushConstants() const {
	return pushConstants;
}
