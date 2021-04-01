#include "CubeShadowMap.h"

#include "VulkanRenderer.h"

using namespace OSK;

CubeShadowMap::CubeShadowMap(RenderAPI* renderer, ContentManager* content) {
	this->renderer = renderer;
	Content = content;
}

CubeShadowMap::~CubeShadowMap() {
	Clear();
}

void CubeShadowMap::Clear() {
	for (auto& i : CubemapTargets)
		SafeDelete(&i);
	SafeDelete(&ShadowsPipeline);

	for (auto& i : UBOs)
		i.Free();

	UBOs.clear();
}

void CubeShadowMap::Create(const Vector2ui& size) {
	Size = size;

	for (uint32_t i = 0; i < 6; i++) {
		CubemapTargets[i] = renderer->CreateNewRenderTarget();
		CubemapTargets[i]->Size = size;
		CubemapTargets[i]->CreateSprite(Content);

		VULKAN::VulkanImageGen::CreateImage(&CubemapTargets[i]->RenderedSprite.Texture2D->Image, Size, CUBE_SHADOW_MAP_FORMAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1);
		VULKAN::VulkanImageGen::CreateImageView(&CubemapTargets[i]->RenderedSprite.Texture2D->Image, CUBE_SHADOW_MAP_FORMAT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
		VULKAN::VulkanImageGen::CreateImageSampler(CubemapTargets[i]->RenderedSprite.Texture2D->Image, CUBE_SHADOW_MAP_FILTER, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);
		//renderer->createDescriptorSets(CubemapTargets[i]->RenderedSprite.Texture2D);
	}

	CreateRenderpass();
	CreateFramebuffers();
	CreateBuffers();
	CreateDescSets(1024);
	CreateGraphicsPipeline();
}

void CubeShadowMap::CreateDescSets(uint32_t maxSets) {
	PointShadowDescriptorLayout = renderer->CreateNewDescriptorLayout();
	PointShadowDescriptorLayout->AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	PointShadowDescriptorLayout->AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	//PointShadowDescriptorLayout->Create(maxSets);
}

void CubeShadowMap::CreateBuffers() {
	VkDeviceSize size = sizeof(PointLightShadowUBO);
	UBOs.resize(renderer->SwapchainImages.size());

	for (uint32_t i = 0; i < UBOs.size(); i++) {
		UBOs[i] = renderer->CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		UBOs[i].Allocate(size);
	}
}

void CubeShadowMap::UpdateBuffers() {
	for (auto& buffer : UBOs) {
		void* data;
		vkMapMemory(renderer->LogicalDevice, buffer.Memory, 0, sizeof(PointLightShadowUBO), 0, &data);
		memcpy(data, ShadowUBO.Matrices.data(), sizeof(PointLightShadowUBO));
		vkUnmapMemory(renderer->LogicalDevice, buffer.Memory);
	}
}

void CubeShadowMap::Update() {
	ShadowUBO.Update(DepthRangeNear, DepthRangeFar, Light->Position);
	UpdateBuffers();
}

void CubeShadowMap::CreateRenderpass() {
	VULKAN::RenderpassAttachment dpthAttachment{};
	dpthAttachment.AddAttachment(VK_FORMAT_D32_SFLOAT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	dpthAttachment.CreateReference(0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	std::vector<VULKAN::RenderpassAttachment> attchments = { };

	CubemapTargets[0]->CreateRenderpass(attchments, &dpthAttachment);
	
	for (auto i : CubemapTargets)
		i->VRenderpass = CubemapTargets[0]->VRenderpass;
}

void CubeShadowMap::CreateFramebuffers() {
	for (auto i : CubemapTargets) {
		i->SetSize(Size.X, Size.Y, false, false);
		i->CreateFramebuffers(4, &i->RenderedSprite.Texture2D->Image.View, 1);
	}
}

void CubeShadowMap::CreateGraphicsPipeline() {
	/*ShadowsPipeline = renderer->CreateNewGraphicsPipeline("shaders/VK_PShadows/vert.spv", "shaders/VK_PShadows/frag.spv");
	ShadowsPipeline->SetViewport({ 0, 0, (float)Size.X, (float)Size.Y });
	ShadowsPipeline->SetRasterizer(VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE); //VK_CULL_MODE_FRONT_BIT VK_CULL_MODE_NONE
	ShadowsPipeline->SetMSAA(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);
	ShadowsPipeline->SetDepthStencil(true);
	ShadowsPipeline->SetPushConstants(VK_SHADER_STAGE_VERTEX_BIT, sizeof(PointLightShadowPushConst));
	ShadowsPipeline->SetLayout(&PointShadowDescriptorLayout->VulkanDescriptorSetLayout);
	ShadowsPipeline->Create(CubemapTargets[0]->VRenderpass);

	CubemapTargets[0]->Pipelines.push_back(ShadowsPipeline);*/
}

/*void CubeShadowMap::CreateDescriptorSet(ModelTexture* texture) {
	if (texture->PointShadowsDescriptorSet != nullptr)
		delete texture->PointShadowsDescriptorSet;

	texture->PointShadowsDescriptorSet = renderer->CreateNewDescriptorSet();
	texture->PointShadowsDescriptorSet->SetDescriptorLayout(PointShadowDescriptorLayout);
	texture->PointShadowsDescriptorSet->AddUniformBuffers(renderer->UniformBuffers, 0, sizeof(UBO));
	texture->PointShadowsDescriptorSet->AddUniformBuffers(UBOs, 1, sizeof(PointLightShadowUBO));
	texture->PointShadowsDescriptorSet->Create();
}*/
