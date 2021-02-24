#include "ShadowMap.h"

#include "VulkanRenderer.h"

#include <ext\matrix_clip_space.hpp>

using namespace OSK;

ShadowMap::ShadowMap(RenderAPI* renderer, ContentManager* content) {
	this->renderer = renderer;
	Content = content;
}

ShadowMap::~ShadowMap() {
	Clear();
}

void ShadowMap::Clear() {
	SafeDelete(&DirShadows);
	SafeDelete(&ShadowsPipeline);

	for (auto& i : DirShadowsUniformBuffers)
		renderer->DestroyBuffer(i);
	DirShadowsUniformBuffers.clear();
}

void ShadowMap::Create(const Vector2ui& size) {
	Size = size;

	DirShadows = renderer->CreateNewRenderTarget();
	DirShadows->Size = Size;
	DirShadows->CreateSprite(Content);

	VULKAN::VulkanImageGen::CreateImage(&DirShadows->RenderedSprite.texture->Albedo, Size, SHADOW_MAP_FORMAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1);
	VULKAN::VulkanImageGen::CreateImageView(&DirShadows->RenderedSprite.texture->Albedo, SHADOW_MAP_FORMAT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
	VULKAN::VulkanImageGen::CreateImageSampler(DirShadows->RenderedSprite.texture->Albedo, SHADOW_MAP_FILTER, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);
	renderer->createDescriptorSets(DirShadows->RenderedSprite.texture);

	CreateRenderpass();
	CreateFramebuffers();
	CreateBuffers();
	CreateDescSets(1024);
	CreateGraphicsPipeline();
}

void ShadowMap::CreateDescSets(uint32_t maxSets) {
	DirShadowDescriptorLayout = renderer->CreateNewDescriptorLayout();
	DirShadowDescriptorLayout->AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	DirShadowDescriptorLayout->AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	DirShadowDescriptorLayout->AddBinding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	DirShadowDescriptorLayout->Create(maxSets);
}

void ShadowMap::CreateBuffers() {
	VkDeviceSize size = sizeof(DirLightShadowUBO);
	DirShadowsUniformBuffers.resize(renderer->SwapchainImages.size());
	for (uint32_t i = 0; i < DirShadowsUniformBuffers.size(); i++)
		renderer->CreateBuffer(DirShadowsUniformBuffers[i], size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void ShadowMap::UpdateBuffers() {
	for (auto& i : DirShadowsUniformBuffers) {
		void* data;
		vkMapMemory(renderer->LogicalDevice, i.Memory, 0, sizeof(DirLightShadowUBO), 0, &data);
		memcpy(data, &DirShadowsUBO, sizeof(DirLightShadowUBO));
		vkUnmapMemory(renderer->LogicalDevice, i.Memory);
	}
}

void ShadowMap::Update() {
	glm::mat4 lightProjection = glm::ortho(-(float)(Size.X / (2 * Density)), (float)(Size.X / (2 * Density)), -(float)(Size.Y / (2 * Density)), (float)(Size.Y / (2 * Density)), DepthRangeFar, DepthRangeNear);
	glm::mat4 lightView = glm::lookAt(Lights->Directional.Direction.ToGLM(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	DirShadowsUBO.DirLightMat = lightProjection * lightView;
}

void ShadowMap::CreateRenderpass() {
	VULKAN::RenderpassAttachment dpthAttachment{};
	dpthAttachment.AddAttachment(VK_FORMAT_D32_SFLOAT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	dpthAttachment.CreateReference(0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	std::vector<VULKAN::RenderpassAttachment> attchments = { };

	DirShadows->CreateRenderpass(attchments, &dpthAttachment);
}

void ShadowMap::CreateFramebuffers() {
	DirShadows->SetSize(Size.X, Size.Y, false, false);
	DirShadows->CreateFramebuffers(4, &DirShadows->RenderedSprite.texture->Albedo.View, 1);
}

void ShadowMap::CreateGraphicsPipeline() {
	ShadowsPipeline = renderer->CreateNewGraphicsPipeline("shaders/VK_Shadows/vert.spv", "shaders/VK_Shadows/frag.spv");
	ShadowsPipeline->SetViewport({ 0, 0, (float)Size.X, (float)Size.Y });
	ShadowsPipeline->SetRasterizer(VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE); //VK_CULL_MODE_FRONT_BIT VK_CULL_MODE_NONE
	ShadowsPipeline->SetMSAA(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);
	ShadowsPipeline->SetDepthStencil(true);
	ShadowsPipeline->SetPushConstants(VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConst3D));
	ShadowsPipeline->SetLayout(&DirShadowDescriptorLayout->VulkanDescriptorSetLayout);
	ShadowsPipeline->Create(DirShadows->VRenderpass);

	DirShadows->Pipelines.push_back(ShadowsPipeline);
}

void ShadowMap::CreateDescriptorSet(Model* model) {
	if (model->texture->DirShadowsDescriptorSet != nullptr)
		delete model->texture->DirShadowsDescriptorSet;

	model->texture->DirShadowsDescriptorSet = renderer->CreateNewDescriptorSet();
	model->texture->DirShadowsDescriptorSet->SetDescriptorLayout(DirShadowDescriptorLayout);
	model->texture->DirShadowsDescriptorSet->AddUniformBuffers(renderer->UniformBuffers, 0, sizeof(UBO));
	model->texture->DirShadowsDescriptorSet->AddUniformBuffers(model->BonesUBOs, 1, sizeof(AnimUBO));
	model->texture->DirShadowsDescriptorSet->AddUniformBuffers(DirShadowsUniformBuffers, 2, sizeof(DirLightShadowUBO));
	model->texture->DirShadowsDescriptorSet->Create();
}