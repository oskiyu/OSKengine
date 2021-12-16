#include "MaterialInstance.h"

#include "MaterialSlotPool.h"
#include "MaterialSlot.h"
#include "Material.h"

using namespace OSK;

MaterialInstance::~MaterialInstance() {

}

MaterialSlot* MaterialInstance::GetMaterialSlot(MaterialSlotTypeId type) {
	return &slots.at(type);
}
MaterialSlotData* MaterialInstance::GetMaterialSlotData(MaterialSlotTypeId type) {
	return owner->GetMaterialSlotData(type, slots.at(type).GetHandler());
}

void MaterialInstance::FlushUpdate() {
	for (auto& i : slots)
		i.second.FlushUpdate();
}

bool MaterialInstance::HasBeenSet() const {
	for (auto i : types)
		if (!owner->GetMaterialSlotData(i, slots.at(i).GetHandler())->HasBeenSet())
			return false;

	return true;
}

size_t MaterialInstance::GetNumberOfSlots() const {
	return types.size();
}

Material* MaterialInstance::GetMaterial() const {
	return owner;
}

void MaterialInstance::AddType(MaterialSlotTypeId type) {
	types.push_back(type);

	slots[type] = MaterialSlot(owner, type);
}
