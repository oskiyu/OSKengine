#include "Material.h"

#include "VulkanRenderer.h"

using namespace OSK;

Material::Material(MaterialPipelineTypeId type, MaterialSystem* owner) {
	pipelineType = type;
	this->owner = owner;
}

Material::~Material() {
	for (auto i : materialSlotPools)
		for (auto j : i.second)
			j.Delete();

	for (auto i : graphicPipelines)
		i.second.Delete();
}

void Material::SetRenderer(RenderAPI* renderer) {
	this->renderer = renderer;
}
void Material::SetPipelineSettings(const MaterialPipelineCreateInfo& info) {
	pipelineInfo = info;

	for (auto i : renderpassesToRegister)
		RegisterRenderpass(i);

	renderpassesToRegister.clear();
}

void Material::RegisterRenderpass(VULKAN::Renderpass* renderpass) {
	if (graphicPipelines.find(renderpass) != graphicPipelines.end())
		return;

	GraphicsPipeline* graphicsPipeline = renderer->CreateNewGraphicsPipeline(pipelineInfo.vertexPath, pipelineInfo.fragmentPath).GetPointer();
	graphicsPipeline->SetViewport({ 0, 0, 2, 2 });
	graphicsPipeline->SetRasterizer(!pipelineInfo.cullFaces, GetVkPolygonMode(pipelineInfo.polygonMode), GetVkCullMode(pipelineInfo.cullMode), GetVkPolygonFrontFace(pipelineInfo.frontFaceType));
	graphicsPipeline->SetMSAA(VK_FALSE, renderer->GetMsaaSamples());
	graphicsPipeline->SetDepthStencil(pipelineInfo.useDepthStencil);

	uint32_t pOffset = 0;
	for (auto i : pipelineInfo.pushConstants) {
		graphicsPipeline->SetPushConstants(GetVulkanShaderBinding(i.shaderStage), (uint32_t)i.size, pOffset);
		pOffset += (uint32_t)i.size;
	}

	graphicsPipeline->SetLayout(owner->GetMaterialPipelineLayout(pipelineType));
	graphicsPipeline->Create(renderpass);

	graphicPipelines[renderpass] = graphicsPipeline;
}
void Material::UnregisterRenderpass(VULKAN::Renderpass* renderpass) {
	if (graphicPipelines.find(renderpass) == graphicPipelines.end())
		return;

	graphicPipelines[renderpass].Delete();

	graphicPipelines.erase(renderpass);
}

GraphicsPipeline* Material::GetGraphicsPipeline(VULKAN::Renderpass* renderpass) const {
	return graphicPipelines.at(renderpass).GetPointer();
}
MaterialPipelineCreateInfo Material::GetMaterialGraphicsPipelineInfo() const {
	return pipelineInfo;
}

OwnedPtr<MaterialInstance> Material::CreateInstance() {
	MaterialInstance* instance = new MaterialInstance();

	for (auto i : materialSlotPools)
		instance->SetMaterialSlot(i.first, GetNextMaterialSlotPool(i.first));

	return instance;
}

MaterialSlotPool* Material::GetNextMaterialSlotPool(MaterialSlotTypeId type) {
	if (materialSlotPools.find(type) == materialSlotPools.end()) {
		materialSlotPools[type] = {};

		materialSlotPools.at(type).push_back(new MaterialSlotPool(renderer, type, owner));


		return materialSlotPools.at(type).back().GetPointer();
	}

	auto& list = materialSlotPools.at(type);

	for (auto i : list)
		if (!i->IsFull())
			return i.GetPointer();

	list.push_back(new MaterialSlotPool(renderer, type, owner));

	return materialSlotPools.at(type).back().GetPointer();
}
