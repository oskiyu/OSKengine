#include "ShadowMap.h"

#include "VulkanRenderer.h"

#include <ext\matrix_clip_space.hpp>

using namespace OSK;

ShadowMap::ShadowMap(RenderAPI* renderer, ContentManager* content) {
	this->renderer = renderer;
	this->content = content;
}

ShadowMap::~ShadowMap() {
	Clear();
}

void ShadowMap::Clear() {
	dirShadowsUniformBuffers.clear();
}

void ShadowMap::Create(const Vector2ui& size) {
	this->size = size;

	dirShadows = renderer->CreateNewRenderTarget();
	dirShadows->SetSize(size.X, size.Y);
	dirShadows->CreateSprite(content);
	dirShadows->renderedSprite.texture->size = size;

	VULKAN::VulkanImageGen::CreateImage(dirShadows->renderedSprite.texture->image.GetPointer(), size, SHADOW_MAP_FORMAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1);
	VULKAN::VulkanImageGen::CreateImageView(dirShadows->renderedSprite.texture->image.GetPointer(), SHADOW_MAP_FORMAT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
	VULKAN::VulkanImageGen::CreateImageSampler(dirShadows->renderedSprite.texture->image.Get(), SHADOW_MAP_FILTER, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);
	dirShadows->renderedSprite.UpdateMaterialTexture();
	
	CreateRenderpass();
	CreateFramebuffers();
	CreateBuffers();
}


void ShadowMap::CreateBuffers() {
	VkDeviceSize size = sizeof(DirLightShadowUBO);
	dirShadowsUniformBuffers.resize(renderer->swapchainImages.size());
	for (uint32_t i = 0; i < dirShadowsUniformBuffers.size(); i++) {
		dirShadowsUniformBuffers[i] = new GpuDataBuffer;
		renderer->AllocateBuffer(dirShadowsUniformBuffers[i].GetPointer(), size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}

void ShadowMap::UpdateBuffers() {
	for (auto& i : dirShadowsUniformBuffers) {
		i->Write(&dirShadowsUBO, sizeof(DirLightShadowUBO));
	}
}

void ShadowMap::Update() {
	glm::mat4 lightProjection = glm::ortho(-(float)(size.X / (2 * density)), (float)(size.X / (2 * density)), -(float)(size.Y / (2 * density)), (float)(size.Y / (2 * density)), depthRangeFar, depthRangeNear);
	glm::mat4 lightView = glm::lookAt(lights->directional.direction.ToGLM(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	dirShadowsUBO.DirLightMat = lightProjection * lightView;
}

void ShadowMap::CreateRenderpass() {
	VULKAN::RenderpassAttachment dpthAttachment{};
	dpthAttachment.AddAttachment(VK_FORMAT_D32_SFLOAT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	dpthAttachment.CreateReference(0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	std::vector<VULKAN::RenderpassAttachment> attchments = { };

	dirShadows->CreateRenderpass(attchments, &dpthAttachment);
}

void ShadowMap::CreateFramebuffers() {
	dirShadows->SetSize(size.X, size.Y, false);
	dirShadows->CreateFramebuffers(4, &dirShadows->renderedSprite.texture->image->view, 1);
}

std::vector<SharedPtr<GpuDataBuffer>>& ShadowMap::GetUniformBuffers() {
	return dirShadowsUniformBuffers;
}

RenderTarget* ShadowMap::GetRenderTarget() {
	return dirShadows.GetPointer();
}

Vector2ui ShadowMap::GetImageSize() const {
	return size;
}

