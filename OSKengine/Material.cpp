#include "Material.h"

#include "VulkanRenderer.h"

using namespace OSK;
using namespace OSK::VULKAN;

Texture* Material::DefaultTexture = nullptr;

Material::~Material() {
	for (auto key : pipelines)
		delete key.second;

	pipelines.clear();

	if (materialLayout)
		delete materialLayout;

	if (pool) {
		pool->Free();
		delete pool;
	}
}

void Material::SetRenderer(RenderAPI* renderer) {
	this->renderer = renderer;
	pool = new MaterialPool(renderer);
}

uint32_t Material::GetBindingIndex(const std::string& name) {
	return bindingNameToBinding[name];
}

void Material::SetPipelineSettings(const MaterialPipelineCreateInfo& info) {
	pipelineInfo = info;

	for (auto i : renderpassesToRegister)
		RegisterRenderpass(i);

	renderpassesToRegister.clear();
}

void Material::SetLayout(MaterialBindingLayout layout) {
	if (materialLayout)
		delete materialLayout;

	materialLayout = renderer->CreateNewDescriptorLayout();
	
	for (auto i : layout) {
		uint32_t index = materialLayout->descriptorLayoutBindings.GetSize();
		materialLayout->AddBinding(GetVulkanBindingType(i.type), GetVulkanShaderBinding(i.stage));
		bindingNameToBinding[i.bindingName] = index;
	}

	materialLayout->Create();
	pool->layout = materialLayout;
}

MaterialInstance* Material::CreateInstance() {
	MaterialInstance* instance = pool->CreateInstance();

	instance->ownerMaterial = this;

	return instance;
}

GraphicsPipeline* Material::GetGraphicsPipeline(VULKAN::Renderpass* renderpass) const {
	return pipelines.at(renderpass);
}

void Material::RegisterRenderpass(Renderpass* renderpass) {
	//if (Pipelines.find(renderpass) == Pipelines.end())
		//return;

	GraphicsPipeline* GPipeline = renderer->CreateNewGraphicsPipeline(pipelineInfo.vertexPath, pipelineInfo.fragmentPath);
	GPipeline->SetViewport({ 0, 0, 2, 2 });
	GPipeline->SetRasterizer(!pipelineInfo.cullFaces, GetVkPolygonMode(pipelineInfo.polygonMode), GetVkCullMode(pipelineInfo.cullMode), GetVkPolygonFrontFace(pipelineInfo.frontFaceType));
	GPipeline->SetMSAA(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);
	GPipeline->SetDepthStencil(pipelineInfo.useDepthStencil);

	size_t pOffset = 0;
	for (auto i : pipelineInfo.pushConstants) {
		GPipeline->SetPushConstants(GetVulkanShaderBinding(i.shaderStage), i.size, pOffset);
		pOffset += i.size;
	}

	GPipeline->SetLayout(&materialLayout->vulkanDescriptorSetLayout);
	GPipeline->Create(renderpass);

	pipelines[renderpass] = GPipeline;
}

MaterialPipelineCreateInfo Material::GetMaterialGraphicsPipelineInfo() const {
	return pipelineInfo;
}

void Material::UnregisterRenderpass(Renderpass* renderpass) {
	if (pipelines.find(renderpass) == pipelines.end())
		return;

	delete pipelines[renderpass];

	pipelines.erase(renderpass);
}
