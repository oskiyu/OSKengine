#include "MaterialLayout.h"

using namespace OSK;

void MaterialLayout::AddSlot(const MaterialLayoutSlot& slot) {
	slots.Insert(slot.name, slot);
	slotNames.Push(slot.name);
}

MaterialLayoutSlot& MaterialLayout::GetSlot(const std::string& name) const {
	return slots.Get(name);
}

const LinkedList<std::string>& MaterialLayout::GetAllSlotNames() const {
	return slotNames;
}
