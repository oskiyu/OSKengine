#include "MaterialLayout.h"

#include "VulkanRenderer.h"

using namespace OSK;

//Añade el descriptor set al pool del último data.
//Si el último data está lleno, se crea un nuevo pool.
MaterialInstance MaterialPool::CreateInstance() {
	if (Datas.size() == 0)
		AddNewData();

	MaterialInstance instance{};

	instance.DSet = Renderer->CreateNewDescriptorSet();

	auto& data = Datas.back();
	instance.DSet->SetDescriptorLayout(data.Layout);
	data.CurrentSize++;

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
	MaterialPoolData newData;
	newData.Layout = Renderer->CreateNewDescriptorLayout();

	for (uint32_t i = 0; i < Bindings.size(); i++)
		newData.Layout->AddBinding(i, GetVulkanBindingType(Bindings[i].Type), GetVulkanShaderBinding(Bindings[i].Stage));

	newData.Layout->Create(newData.MaxSize);

	Datas.push_back(newData);
}

VkDescriptorType MaterialPool::GetVulkanBindingType(MaterialBindingType type) const {
	switch (type) {
	case MaterialBindingType::DATA_BUFFER:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	case MaterialBindingType::DYNAMIC_DATA_BUFFER:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;

	case MaterialBindingType::TEXTURE:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

	}

	return VK_DESCRIPTOR_TYPE_END_RANGE;
}

VkShaderStageFlags MaterialPool::GetVulkanShaderBinding(MaterialBindingShaderStage type) const {
	switch (type) {
	case MaterialBindingShaderStage::VERTEX:
		return VK_SHADER_STAGE_VERTEX_BIT;

	case MaterialBindingShaderStage::FRAGMENT:
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	}

	return VK_SHADER_STAGE_ALL;
}
