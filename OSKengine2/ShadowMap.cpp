#include "ShadowMap.h"

#include "Lights.h"

#include "Format.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "Material.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtc/matrix_transform.hpp>
#include <gtx/projection.hpp>
#include <glm.hpp>
#include <ext/matrix_clip_space.hpp>

using namespace OSK;
using namespace OSK::GRAPHICS;

void ShadowMap::Create(const Vector2ui& imageSize) {
	GpuImageSamplerDesc depthSampler{};
	depthSampler.mipMapMode = GpuImageMipmapMode::NONE;
	shadowsTarget.SetColorImageSampler(depthSampler);
	shadowsTarget.SetDepthImageSampler(depthSampler);
	shadowsTarget.SetDepthImageUsage(GpuImageUsage::SAMPLED);
	shadowsTarget.Create(imageSize, Format::RGBA8_UNORM, Format::D32S8_SFLOAT_SUINT);

	shadowsGenMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_shadows.json");
	shadowsGenMaterialInstance = shadowsGenMaterial->CreateInstance().GetPointer();

	struct Ubo {
		alignas(16) glm::mat4 lightMatrix;
	};

	lightUniformBuffer = Engine::GetRenderer()->GetMemoryAllocator()->CreateUniformBuffer(sizeof(Ubo)).GetPointer();
	lightUniformBuffer->MapMemory();
	lightUniformBuffer->Write(glm::mat4(1.0f));
	lightUniformBuffer->Unmap();

	shadowsGenMaterialInstance->GetSlot("global")->SetUniformBuffer("dirLight", GetDirLightMatrixUniformBuffer());
	shadowsGenMaterialInstance->GetSlot("global")->FlushUpdate();
}

void ShadowMap::SetDirectionalLight(const DirectionalLight& dirLight) {
	OSK_ASSERT(shadowsGenMaterial != nullptr, "Se debe crear el ShadowMap antes de poder establecer su luz direccional.");
	OSK_ASSERT(shadowsGenMaterialInstance.HasValue(), "Se debe crear el ShadowMap antes de poder establecer su luz direccional.");
	OSK_ASSERT(lightUniformBuffer.HasValue(), "Se debe crear el ShadowMap antes de poder establecer su luz direccional.");

	lightDirection = dirLight.direction;

	if (lightOrigin == Vector3f(0.0f))
		SetLightOrigin(lightDirection * 5.0f);

	UpdateLightMatrixBuffer();
}

void ShadowMap::UpdateLightMatrixBuffer() {
	const glm::mat4 lightMatrix =
		glm::ortho(
			-lightArea.X / 2.0f, lightArea.X / 2.0f,
			-lightArea.Y / 2.0f, lightArea.Y / 2.0f,
			nearPlane, farPlane)
		* glm::lookAt((lightOrigin - lightDirection).ToGLM(), lightOrigin.ToGLM(), glm::vec3(0.0f, 1.0f, 0.0f));

	struct {
		alignas(16) glm::mat4 lightMatrix;
	} ubo{ .lightMatrix = lightMatrix };
	lightUniformBuffer->ResetCursor();
	lightUniformBuffer->MapMemory();
	lightUniformBuffer->Write(ubo);
	lightUniformBuffer->Unmap();
}

void ShadowMap::SetLightArea(const Vector2f& area) {
	lightArea = area;
}

void ShadowMap::SetNearPlane(float nearPlane) {
	this->nearPlane = nearPlane;
}

void ShadowMap::SetFarPlane(float farPlane) {
	this->farPlane = farPlane;
}

void ShadowMap::SetLightOrigin(const Vector3f& origin) {
	lightOrigin = origin;

	UpdateLightMatrixBuffer();
}

Vector3f ShadowMap::GetCurrentLigthOrigin() const {
	return lightOrigin;
}

GpuImage* ShadowMap::GetShadowImage(TSize index) const {
	return shadowsTarget.GetDepthImage(index);
}

RenderTarget* ShadowMap::GetShadowsRenderTarget() {
	return &shadowsTarget;
}

Material* ShadowMap::GetShadowsMaterial() const {
	return shadowsGenMaterial;
}

MaterialInstance* ShadowMap::GetShadowsMaterialInstance() const {
	return shadowsGenMaterialInstance.GetPointer();
}

IGpuUniformBuffer* ShadowMap::GetDirLightMatrixUniformBuffer() const {
	return lightUniformBuffer.GetPointer();
}
