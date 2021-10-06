#include "MaterialInstance.h"

#include "MaterialSlotPool.h"
#include "MaterialSlot.h"

using namespace OSK;

MaterialInstance::~MaterialInstance() {

}

MaterialSlot* MaterialInstance::GetMaterialSlot(MaterialSlotTypeId type) {
	return slotPools.at(type)->GetMaterialSlot(slotIds.at(type));
}

void MaterialInstance::SetMaterialSlot(MaterialSlotTypeId type, MaterialSlotPool* pool) {
	slotIds[type] = pool->GetNextMaterialSlot();
	slotPools[type] = pool;

	types.push_back(type);

	numberOfSlots++;
}

void MaterialInstance::FlushUpdate() {
	for (auto i : types)
		slotPools[i]->GetMaterialSlot(slotIds[i])->FlushUpdate();
}

bool MaterialInstance::HasBeenSet() const {
	for (auto i : types)
		if (!slotPools.at(i)->GetMaterialSlot(slotIds.at(i))->HasBeenSet())
			return false;

	return true;
}

size_t MaterialInstance::GetNumberOfSlots() const {
	return numberOfSlots;
}
