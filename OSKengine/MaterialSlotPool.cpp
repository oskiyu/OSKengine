#include "MaterialSlotPool.h"

#include "VulkanRenderer.h"
#include "MaterialSlot.h"
#include "MaterialSystem.h"

using namespace OSK;

MaterialSlotPool::MaterialSlotPool(RenderAPI* renderer, MaterialSlotTypeId type, MaterialSystem* system) {
	this->renderer = renderer;
	this->type = type;

	pool = renderer->CreateNewDescriptorPool().GetPointer();

	auto layout = system->GetDescriptorLayout(type);

	pool->SetLayout(layout);
	pool->Create(PoolSize);

	for (uint32_t i = 0; i < PoolSize; i++)
		freeSpaces.push(i);

	materialSlots.resize(PoolSize);
	descriptorSets.resize(PoolSize);

	for (uint32_t i = 0; i < PoolSize; i++) {
		materialSlots[i] = new MaterialSlot();
		materialSlots[i]->index = i;
		materialSlots[i]->pool = this;
		materialSlots[i]->slotType = type;

		descriptorSets[i] = renderer->CreateNewDescriptorSet();
		descriptorSets[i]->Create(layout, pool.GetPointer());
	}
}

MaterialSlotPool::~MaterialSlotPool() {
	for (auto i : materialSlots)
		i.Delete();
	for (auto i : descriptorSets)
		i.Delete();

	pool.Delete();
}

uint32_t MaterialSlotPool::GetNextMaterialSlot() {
	uint32_t output = freeSpaces.top();
	freeSpaces.pop();

	return output;
}

MaterialSlot* MaterialSlotPool::GetMaterialSlot(uint32_t id) {
	return materialSlots[id].GetPointer();
}

DescriptorSet* MaterialSlotPool::GetDescriptorSet(uint32_t id) {
	return descriptorSets[id].GetPointer();
}

bool MaterialSlotPool::IsEmpty() const {
	return freeSpaces.empty();
}

bool MaterialSlotPool::IsFull() const {
	return freeSpaces.size() == PoolSize - 1;
}

void MaterialSlotPool::FreeSlot(uint32_t id) {
	for (const auto& i : freeSpaces._Get_container()) {
		if (i == id) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "Se ha intentado liberar un material slot que ya estaba liberado.");
			
			throw std::runtime_error("Se ha intentado liberar un material slot que ya estaba liberado.");
		}
	}

	freeSpaces.push(id);
}
