#include "Material.h"

#include "VulkanRenderer.h"

using namespace OSK;
using namespace OSK::VULKAN;

Texture* Material::DefaultTexture = nullptr;

Material::~Material() {
	for (auto key : Pipelines)
		delete key.second;

	Pipelines.clear();

	if (MLayout)
		delete MLayout;

	if (Pool) {
		Pool->Free();
		delete Pool;
	}
}

void Material::SetPipelineSettings(const MaterialPipelineCreateInfo& info) {
	PipelineInfo = info;

	for (auto i : RenderpassesToRegister)
		RegisterRenderpass(i);

	RenderpassesToRegister.clear();
}

void Material::SetLayout(MaterialBindingLayout layout) {
	if (MLayout)
		delete MLayout;

	MLayout = Renderer->CreateNewDescriptorLayout();
	
	for (auto i : layout) {
		uint32_t index = MLayout->DescriptorLayoutBindings.GetSize();
		MLayout->AddBinding(GetVulkanBindingType(i.Type), GetVulkanShaderBinding(i.Stage));
		BindingNameToBinding[i.BindingName] = index;
	}

	MLayout->Create();
	Pool->Layout = MLayout;
}

MaterialInstance* Material::CreateInstance() {
	MaterialInstance* instance = Pool->CreateInstance();

	instance->OwnerMaterial = this;

	return instance;
}

GraphicsPipeline* Material::GetGraphicsPipeline(VULKAN::Renderpass* renderpass) const {
	return Pipelines.at(renderpass);
}

void Material::RegisterRenderpass(Renderpass* renderpass) {
	//if (Pipelines.find(renderpass) == Pipelines.end())
		//return;

	GraphicsPipeline* GPipeline = Renderer->CreateNewGraphicsPipeline(PipelineInfo.VertexPath, PipelineInfo.FragmentPath);
	GPipeline->SetViewport({ 0, 0, 2, 2 });
	GPipeline->SetRasterizer(!PipelineInfo.CullFaces, GetVkPolygonMode(PipelineInfo.PMode), GetVkCullMode(PipelineInfo.CullMode), GetVkPolygonFrontFace(PipelineInfo.FrontFaceType));
	GPipeline->SetMSAA(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);
	GPipeline->SetDepthStencil(PipelineInfo.UseDepthStencil);

	size_t pOffset = 0;
	for (auto i : PipelineInfo.PushConstants) {
		GPipeline->SetPushConstants(GetVulkanShaderBinding(i.ShaderStage), i.Size, pOffset);
		pOffset += i.Size;
	}

	GPipeline->SetLayout(&MLayout->VulkanDescriptorSetLayout);
	GPipeline->Create(renderpass);

	Pipelines[renderpass] = GPipeline;
}

MaterialPipelineCreateInfo Material::GetMaterialGraphicsPipelineInfo() const {
	return PipelineInfo;
}

void Material::UnregisterRenderpass(Renderpass* renderpass) {
	if (Pipelines.find(renderpass) == Pipelines.end())
		return;

	delete Pipelines[renderpass];

	Pipelines.erase(renderpass);
}
