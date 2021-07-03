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
	dirShadows->CreateRenderpass();

	dirShadows->clearColor = Color::WHITE();

	CreateBuffers();
}


void ShadowMap::CreateBuffers() {
	VkDeviceSize size = sizeof(DirLightShadowUBO);
	dirShadowsUniformBuffers.resize(renderer->swapchain->GetImageCount());
	for (uint32_t i = 0; i < dirShadowsUniformBuffers.size(); i++) {
		dirShadowsUniformBuffers[i] = new GpuDataBuffer;
		renderer->AllocateBuffer(dirShadowsUniformBuffers[i].GetPointer(), size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}

void ShadowMap::UpdateBuffers() {
	for (auto& i : dirShadowsUniformBuffers)
		i->Write(&dirShadowsUBO, sizeof(DirLightShadowUBO));
}

void ShadowMap::Update() {
	glm::mat4 lightProjection = glm::ortho(-(float)(size.X / (2 * density)), (float)(size.X / (2 * density)), -(float)(size.Y / (2 * density)), (float)(size.Y / (2 * density)), depthRangeFar, depthRangeNear);
	glm::mat4 lightView = glm::lookAt(lights->directional.direction.ToGLM(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	dirShadowsUBO.DirLightMat = lightProjection * lightView;
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

