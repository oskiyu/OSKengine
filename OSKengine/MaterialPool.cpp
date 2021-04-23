#include "MaterialPool.h"

#include "VulkanRenderer.h"

using namespace OSK;

MaterialPool::MaterialPool(RenderAPI* renderer) : Renderer(renderer) {

}

MaterialPool::~MaterialPool() {
	//Free();
}

void MaterialPool::AddBinding(MaterialBindingType binding, MaterialBindingShaderStage shaderStage) {
	Bindings.push_back({ binding, shaderStage });
}

//Añade el descriptor set al pool del último data.
//Si el último data está lleno, se crea un nuevo pool.
MaterialInstance* MaterialPool::CreateInstance() {
	if (Datas.size() == 0)
		AddNewData();

	MaterialInstance* instance = new MaterialInstance;

	auto& data = Datas.back();

	instance->DSet = data.GetNextDescriptorSet();
	instance->OwnerPool = this;

	if (data.IsFull())
		AddNewData();

	return instance;
}

void MaterialPool::Free() {
	for (auto& i : Datas)
		i.Free();

	Datas.clear();
}

void MaterialPool::AddNewData() {
	MaterialPoolData newData(Renderer);
	newData.DPool = Renderer->CreateNewDescriptorPool();
	newData.DPool->SetLayout(Layout);
	newData.SetLayout(Layout);

	for (uint32_t i = 0; i < Bindings.size(); i++)
		newData.DPool->AddBinding(GetVulkanBindingType(Bindings[i].Type));

	newData.DPool->Create(MaterialPoolSize);
	newData.BaseSize = Datas.size() * MaterialPoolSize;

	Datas.push_back(newData);
}

void MaterialPool::SetLayout(const MaterialBindingLayout& layout) {
	Bindings = layout;
}

DescriptorSet* MaterialPool::GetDSet(uint32_t index) {
	uint32_t bucket = index / MaterialPoolSize;
	uint32_t offset = index % MaterialPoolSize;

	return Datas[bucket].DescriptorSets[offset];
}

void MaterialPool::FreeSet(uint32_t index) {
	uint32_t bucket = index / MaterialPoolSize;

	Datas[bucket].FreeSpaces.Push(index);
}
