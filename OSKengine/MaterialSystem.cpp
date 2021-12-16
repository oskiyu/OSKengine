#include "MaterialSystem.h"

#include "VulkanRenderer.h"

using namespace OSK;
using namespace OSK::VULKAN;

MaterialSystem::MaterialSystem(RenderAPI* renderer) {
	this->renderer = renderer;

	//MPIPE_2D
	SetDescriptorLayout(MPIPE_2D, MSLOT_CAMERA_2D, { {MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Camera"} });
	SetDescriptorLayout(MPIPE_2D, MSLOT_TEXTURE_2D, { {MaterialBindingType::TEXTURE, MaterialBindingShaderStage::FRAGMENT, "Texture"} });

	//MPIPE_3D
	SetDescriptorLayout(MPIPE_3D, MSLOT_CAMERA_3D, { { MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Camera" } });
	SetDescriptorLayout(MPIPE_3D, MSLOT_SCENE_3D, {
		{ MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "DirLightMat" },
		{ MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::FRAGMENT, "Lights" },
		{ MaterialBindingType::TEXTURE, MaterialBindingShaderStage::FRAGMENT, "ShadowsTexture" }
		});
	SetDescriptorLayout(MPIPE_3D, MSLOT_PER_MODEL_3D, {
		{ MaterialBindingType::TEXTURE, MaterialBindingShaderStage::FRAGMENT, "Albedo" },
		{ MaterialBindingType::TEXTURE, MaterialBindingShaderStage::FRAGMENT, "Specular" }
		});
	SetDescriptorLayout(MPIPE_3D, MSLOT_PER_INSTANCE_3D, { { MaterialBindingType::DYNAMIC_DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Bones" } });

	//MPIPE_SHADOWS3D
	SetDescriptorLayout(MPIPE_SHADOWS3D, MSLOT_SHADOWS_3D_CAMERA, { {MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Camera"} });
	SetDescriptorLayout(MPIPE_SHADOWS3D, MSLOT_SHADOWS_3D_SCENE, { {MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "DirLightMat"} });
	SetDescriptorLayout(MPIPE_SHADOWS3D, MSLOT_SHADOWS_3D_BONES, { {MaterialBindingType::DYNAMIC_DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Bones"} });

	//MPIPE_POSTPROCESS
	SetDescriptorLayout(MPIPE_POSTPROCESS, MSLOT_TEXTURE_2D, { {MaterialBindingType::TEXTURE, MaterialBindingShaderStage::FRAGMENT, "Texture"} });

	//MPIPE_SKYBOX
	SetDescriptorLayout(MPIPE_SKYBOX, MSLOT_SKYBOX_CAMERA, { {MaterialBindingType::DATA_BUFFER, MaterialBindingShaderStage::VERTEX, "Camera"} });
	SetDescriptorLayout(MPIPE_SKYBOX, MSLOT_SKYBOX_TEXTURE, { {MaterialBindingType::TEXTURE, MaterialBindingShaderStage::FRAGMENT, "Cubemap"} });
}

MaterialSystem::~MaterialSystem() {
	for (auto i : materials)
		i.second.Delete();
}

void MaterialSystem::RegisterMaterial(MaterialPipelineTypeId type) {
	Material* material = new Material(type, this);
	material->SetRenderer(renderer);
	material->owner = this;

	for (auto i : renderpasses)
		material->renderpassesToRegister.push_back(i);

	if (materials.find(type) != materials.end())
		materials.at(type).Delete();

	materials[type] = material;
}

Material* MaterialSystem::GetMaterial(MaterialPipelineTypeId type) {
	if (materials.find(type) == materials.end())
		return nullptr;

	return materials[type].GetPointer();
}

void MaterialSystem::RegisterRenderpass(Renderpass* renderpass) {
	for (auto& i : materials)
		i.second->RegisterRenderpass(renderpass);

	renderpasses.push_back(renderpass);
}

void MaterialSystem::UnregisterRenderpass(Renderpass* renderpass) {
	for (auto& i : materials)
		i.second->UnregisterRenderpass(renderpass);

	renderpasses.remove(renderpass);
}

void MaterialSystem::SetDescriptorLayout(MaterialPipelineTypeId mPipeline, MaterialSlotTypeId type, MaterialBindingLayout layout) {
	if (materials.find(mPipeline) == materials.end())
		RegisterMaterial(mPipeline);

	uint32_t set = 0;
	if (pipelinesLayouts.find(mPipeline) != pipelinesLayouts.end())
		set = pipelinesLayouts.at(mPipeline).size();

	DescriptorLayout* descLayout = renderer->CreateNewDescriptorLayout().GetPointer();

	for (const auto& i : layout)
		descLayout->AddBinding(i.type, i.stage, i.bindingName);

	descLayout->Create();

	descriptorLayouts[type] = descLayout;
	pipelinesLayouts[mPipeline].push_back(descLayout->vulkanDescriptorSetLayout);

	materials[mPipeline]->materialSlotPools[type] = {};
	materials[mPipeline]->setNumber[type] = set;
}

uint32_t MaterialSystem::GetBindingFromName(MaterialSlotTypeId type, const std::string& name) const {
	return descriptorLayouts.at(type)->GetBindingFromName(name);
}

DescriptorLayout* MaterialSystem::GetDescriptorLayout(MaterialSlotTypeId type) {
	return descriptorLayouts.at(type).GetPointer();
}

const std::vector<VkDescriptorSetLayout>& MaterialSystem::GetMaterialPipelineLayout(MaterialPipelineTypeId pipeline) const {
	return pipelinesLayouts.at(pipeline);
}
