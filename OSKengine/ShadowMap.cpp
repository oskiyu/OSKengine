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
	Memory::SafeDelete(&DirShadows);

	for (auto& i : DirShadowsUniformBuffers)
		i.Free();

	DirShadowsUniformBuffers.clear();
}

void ShadowMap::Create(const Vector2ui& size) {
	Size = size;

	DirShadows = renderer->CreateNewRenderTarget();
	DirShadows->Size = Size;
	DirShadows->CreateSprite(Content);
	DirShadows->RenderedSprite.Texture2D->Size = Size;

	VULKAN::VulkanImageGen::CreateImage(&DirShadows->RenderedSprite.Texture2D->Image, Size, SHADOW_MAP_FORMAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1);
	VULKAN::VulkanImageGen::CreateImageView(&DirShadows->RenderedSprite.Texture2D->Image, SHADOW_MAP_FORMAT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
	VULKAN::VulkanImageGen::CreateImageSampler(DirShadows->RenderedSprite.Texture2D->Image, SHADOW_MAP_FILTER, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);
	DirShadows->RenderedSprite.UpdateMaterialTexture();
	
	CreateRenderpass();
	CreateFramebuffers();
	CreateBuffers();
}


void ShadowMap::CreateBuffers() {
	VkDeviceSize size = sizeof(DirLightShadowUBO);
	DirShadowsUniformBuffers.resize(renderer->SwapchainImages.size());
	for (uint32_t i = 0; i < DirShadowsUniformBuffers.size(); i++) {
		DirShadowsUniformBuffers[i] = renderer->CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		DirShadowsUniformBuffers[i].Allocate(size);
	}
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
	DirShadows->SetSize(Size.X, Size.Y, false);
	DirShadows->CreateFramebuffers(4, &DirShadows->RenderedSprite.Texture2D->Image.View, 1);
}
