#include "Material.h"

#include "VulkanRenderer.h"

using namespace OSK;
using namespace OSK::VULKAN;

Texture* Material::DefaultTexture = nullptr;

Material::~Material() {
	for (auto key : pipelines)
		delete key.second;

	pipelines.clear();
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
	materialLayout.Delete();

	materialLayout = renderer->CreateNewDescriptorLayout();
	
	for (auto i : layout) {
		uint32_t index = (uint32_t)materialLayout->descriptorLayoutBindings.GetSize();
		materialLayout->AddBinding(GetVulkanBindingType(i.type), GetVulkanShaderBinding(i.stage));
		bindingNameToBinding[i.bindingName] = index;
	}

	materialLayout->Create();
	pool->layout = materialLayout.GetPointer();
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
	if (pipelines.find(renderpass) != pipelines.end())
		return;

	GraphicsPipeline* graphicsPipeline = renderer->CreateNewGraphicsPipeline(pipelineInfo.vertexPath, pipelineInfo.fragmentPath);
	graphicsPipeline->SetViewport({ 0, 0, 2, 2 });
	graphicsPipeline->SetRasterizer(!pipelineInfo.cullFaces, GetVkPolygonMode(pipelineInfo.polygonMode), GetVkCullMode(pipelineInfo.cullMode), GetVkPolygonFrontFace(pipelineInfo.frontFaceType));
	graphicsPipeline->SetMSAA(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);
	graphicsPipeline->SetDepthStencil(pipelineInfo.useDepthStencil);

	uint32_t pOffset = 0;
	for (auto i : pipelineInfo.pushConstants) {
		graphicsPipeline->SetPushConstants(GetVulkanShaderBinding(i.shaderStage), (uint32_t)i.size, pOffset);
		pOffset += (uint32_t)i.size;
	}

	graphicsPipeline->SetLayout(&materialLayout->vulkanDescriptorSetLayout);
	graphicsPipeline->Create(renderpass);

	pipelines[renderpass] = graphicsPipeline;
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
