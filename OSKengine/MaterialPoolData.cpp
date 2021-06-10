#include "MaterialPoolData.h"

#include "VulkanRenderer.h"

using namespace OSK;

MaterialPoolData::MaterialPoolData(RenderAPI* renderer, uint32_t baseSize) {
	this->renderer = renderer;

	freeSpaces.Allocate(MaterialPoolSize);
	this->baseSize = baseSize;

	descriptorPool = renderer->CreateNewDescriptorPool();

	for (uint32_t i = 0; i < MaterialPoolSize; i++)
		descriptorSets.push_back(renderer->CreateNewDescriptorSet());
}

MaterialPoolData::~MaterialPoolData() {

}

void MaterialPoolData::SetBindings(const std::vector<VkDescriptorType>& bindings) {
	for (uint32_t i = 0; i < bindings.size(); i++)
		descriptorPool->AddBinding(bindings[i]);

	descriptorPool->Create(MaterialPoolSize);
}

DescriptorSet* MaterialPoolData::GetDescriptorSet(uint32_t localIndex) {
	OSK_ASSERT(localIndex >= 0 && localIndex < descriptorSets.size(), "Se ha intentado acceder a un descriptor set que no se encuentra en este material pool data.");

	return descriptorSets[localIndex];
}

void MaterialPoolData::FreeDescriptorSet(uint32_t localIndex) {
	OSK_ASSERT(localIndex >= 0 && localIndex < descriptorSets.size(), "Se ha intentado liberar un descriptor set que no se encuentra en este material pool data.");

	freeSpaces.Push(localIndex);
}

void MaterialPoolData::Free() {
	for (uint32_t i = 0; i < MaterialPoolSize; i++)
		delete descriptorSets[i];

	descriptorSets.clear();

	descriptorPool.Delete();

	freeSpaces.Free();
}

void MaterialPoolData::SetLayout(DescriptorLayout* layout) {
	descriptorPool = renderer->CreateNewDescriptorPool();
	descriptorPool->SetLayout(layout);
	descriptorPool->Create(MaterialPoolSize);

	for (uint32_t i = 0; i < MaterialPoolSize; i++)
		descriptorSets[i]->Create(layout, descriptorPool.GetPointer());
}

uint32_t MaterialPoolData::GetNextDescriptorSet() {
	uint32_t index = 0;

	if (freeSpaces.IsEmpty()) {
		index = currentSize;
	}
	else {
		index = freeSpaces.Pop();
	}

	currentSize++;

	return index + baseSize;
}

bool MaterialPoolData::IsFull() const {
	return currentSize == MaterialPoolSize - 1;
}
