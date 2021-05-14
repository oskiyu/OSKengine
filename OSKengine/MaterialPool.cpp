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

	auto& data = datas.back();

	instance->descriptorSetIndex = data.GetNextDescriptorSet();
	instance->ownerPool = this;

	if (data.IsFull())
		AddNewData();

	return instance;
}

void MaterialPool::Free() {
	for (auto& i : datas)
		i.Free();

	datas.clear();
}

void MaterialPool::AddNewData() {
	MaterialPoolData newData(renderer, datas.size() * MaterialPoolSize);
	newData.SetLayout(layout);

	std::vector<VkDescriptorType> nativeBindings;
	for (uint32_t i = 0; i < bindings.size(); i++)
		nativeBindings.push_back(GetVulkanBindingType(bindings[i].type));

	newData.SetBindings(nativeBindings);

	datas.push_back(newData);
}

void MaterialPool::SetLayout(const MaterialBindingLayout& layout) {
	bindings = layout;
}

DescriptorSet* MaterialPool::GetDSet(uint32_t index) {
	uint32_t bucket = index / MaterialPoolSize;
	uint32_t offset = index % MaterialPoolSize;

	return datas[bucket].GetDescriptorSet(offset);
}

void MaterialPool::FreeSet(uint32_t index) {
	uint32_t bucket = index / MaterialPoolSize;

	datas[bucket].FreeDescriptorSet(index);
}
