#include "MaterialPool.h"

#include "VulkanRenderer.h"

using namespace OSK;

MaterialPool::MaterialPool(RenderAPI* renderer) {
	this->renderer = renderer;
}

MaterialPool::~MaterialPool() {
	Free();
}

void MaterialPool::AddBinding(MaterialBindingType binding, MaterialBindingShaderStage shaderStage) {
	bindings.push_back({ binding, shaderStage });
}

//Añade el descriptor set al pool del último data.
//Si el último data está lleno, se crea un nuevo pool.
MaterialInstance* MaterialPool::CreateInstance() {
	if (datas.size() == 0)
		AddNewData();

	MaterialInstance* instance = new MaterialInstance;

	instance->descriptorSetIndex = datas.back()->GetNextDescriptorSet();
	instance->ownerPool = this;

	if (datas.back()->IsFull())
		AddNewData();

	return instance;
}

void MaterialPool::Free() {
	for (auto& i : datas) {
		i->Free();

		delete i;
	}

	datas.clear();
}

void MaterialPool::AddNewData() {
	MaterialPoolData* newData = new MaterialPoolData(renderer, (uint32_t)datas.size() * MaterialPoolSize);
	newData->SetLayout(layout);

	std::vector<VkDescriptorType> nativeBindings;
	for (uint32_t i = 0; i < bindings.size(); i++)
		nativeBindings.push_back(GetVulkanBindingType(bindings[i].type));

	newData->SetBindings(nativeBindings);

	datas.emplace_back(newData);
}

void MaterialPool::SetLayout(const MaterialBindingLayout& layout) {
	bindings = layout;
}

DescriptorSet* MaterialPool::GetDSet(uint32_t index) {
	uint32_t bucket = index / MaterialPoolSize;
	uint32_t offset = index % MaterialPoolSize;

	return datas[bucket]->GetDescriptorSet(offset);
}

void MaterialPool::FreeSet(uint32_t index) {
	uint32_t bucket = index / MaterialPoolSize;

	datas[bucket]->FreeDescriptorSet(index);
}
