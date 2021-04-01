#include "MaterialPoolData.h"

#include "VulkanRenderer.h"

using namespace OSK;

MaterialPoolData::MaterialPoolData(RenderAPI* renderer) {
	Renderer = renderer;

	FreeSpaces.Allocate(MaterialPoolSize);

	for (uint32_t i = 0; i < MaterialPoolSize; i++) {
		DescriptorSets.push_back(renderer->CreateNewDescriptorSet());
	}
}

MaterialPoolData::~MaterialPoolData() {
	//Free();
}

void MaterialPoolData::Free() {
	for (uint32_t i = 0; i < MaterialPoolSize; i++) {
		delete DescriptorSets[i];
	}
	DescriptorSets.clear();

	if (DPool) {
		delete DPool;
	}

	FreeSpaces.Free();
}

void MaterialPoolData::SetLayout(DescriptorLayout* layout) {
	DPool = Renderer->CreateNewDescriptorPool();
	DPool->SetLayout(layout);
	DPool->Create(MaterialPoolSize);

	for (uint32_t i = 0; i < MaterialPoolSize; i++) {
		DescriptorSets[i]->Create(layout, DPool);
	}
}

uint32_t MaterialPoolData::GetNextDescriptorSet() {
	uint32_t index = 0;

	if (FreeSpaces.IsEmpty()) {
		index = CurrentSize;
	}
	else {
		index = FreeSpaces.Pop();
	}

	CurrentSize++;

	return index + BaseSize;
}

bool MaterialPoolData::IsFull() const {
	return CurrentSize == MaterialPoolSize - 1;
}