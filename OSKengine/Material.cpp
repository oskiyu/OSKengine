#include "Material.h"

#include "VulkanRenderer.h"
#include "MaterialSystem.h"

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

uint32_t Material::GetBindingNumber(MaterialSlotTypeId type, const std::string& name) const {
	return owner->GetBindingFromName(type, name);
}

void Material::RegisterRenderpass(VULKAN::Renderpass* renderpass) {
	if (graphicPipelines.find(renderpass) != graphicPipelines.end())
		return;

	GraphicsPipeline* graphicsPipeline = renderer->CreateNewGraphicsPipeline(pipelineInfo.vertexPath, pipelineInfo.fragmentPath).GetPointer();
	graphicsPipeline->SetViewport({ 0, 0, 2, 2 });
	graphicsPipeline->SetRasterizer(!pipelineInfo.cullFaces, GetVkPolygonMode(pipelineInfo.polygonMode), GetVkCullMode(pipelineInfo.cullMode), GetVkPolygonFrontFace(pipelineInfo.frontFaceType));
	graphicsPipeline->SetMsaa(VK_FALSE, renderer->GetMsaaSamples());
	graphicsPipeline->SetDepthStencil(pipelineInfo.useDepthStencil);

	uint32_t pOffset = 0;
	for (auto i : pipelineInfo.pushConstants) {
		graphicsPipeline->SetPushConstants(GetVulkanShaderBinding(i.shaderStage), (uint32_t)i.size, pOffset);
		pOffset += (uint32_t)i.size;
	}

	graphicsPipeline->SetVertexType(pipelineInfo.type);
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
	instance->owner = this;

	for (auto i : materialSlotPools)
		instance->AddType(i.first);

	return instance;
}

MaterialSlotPool* Material::GetNextMaterialSlotPool(MaterialSlotTypeId type) {
	if (materialSlotPools.find(type) == materialSlotPools.end()) {
		materialSlotPools[type] = {};

		materialSlotPools.at(type).push_back(new MaterialSlotPool(renderer, type, owner, this));

		return materialSlotPools.at(type).back().GetPointer();
	}

	auto& list = materialSlotPools.at(type);

	for (auto i : list)
		if (!i->IsFull())
			return i.GetPointer();

	list.push_back(new MaterialSlotPool(renderer, type, owner, this));

	return materialSlotPools.at(type).back().GetPointer();
}

uint32_t Material::GetDescriptorSetNumber(MaterialSlotTypeId type) const {
	return setNumber.at(type);
}

MaterialSlotData* Material::GetMaterialSlotData(MaterialSlotTypeId type, MaterialSlotHandler handler) const {
	return materialSlots.at(type).at(handler);
}
MaterialSlotHandler Material::GetHandler(MaterialSlotTypeId type, const MaterialSlotBindings& bindings) {
	MaterialSlotHandler handler;
	
	auto typeIt = materialSlots.find(type);
	auto nextHanlderIt = nextHandlers.find(type);
	if (nextHanlderIt == nextHandlers.end()) {
		nextHandlers[type] = 0;

		handler = 0;
	}
	else {
		handler = nextHanlderIt->second;
	}

	if (typeIt == materialSlots.end()) {
		materialSlots[type] = {};
		handler = nextHandlers.at(type);

		auto newSlot = GetNextMaterialSlot(type);
		materialSlots[type][nextHandlers.at(type)] = newSlot.pool->GetMaterialSlot(newSlot.id);
		newSlot.pool->GetMaterialSlot(newSlot.id)->bindings = bindings;

		nextHandlers.at(type)++;

		return handler;
	}

	auto& map = (*typeIt).second;
	auto slotIt = map.begin();
	while (slotIt != map.end()) {
		auto& entry = *slotIt;

		if (entry.second->bindings == bindings)
			return entry.first;

		++slotIt;
	}

	auto newSlot = GetNextMaterialSlot(type);
	materialSlots[type][nextHandlers.at(type)] = newSlot.pool->GetMaterialSlot(newSlot.id);
	newSlot.pool->GetMaterialSlot(newSlot.id)->bindings = bindings;

	handler = nextHandlers.at(type);
	nextHandlers.at(type)++;

	return handler;
}

PoolIdPair Material::GetNextMaterialSlot(MaterialSlotTypeId type) {
	auto pool = GetNextMaterialSlotPool(type);

	return { pool, pool->GetNextMaterialSlot() };
}

