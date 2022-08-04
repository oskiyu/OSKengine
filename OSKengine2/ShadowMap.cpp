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

#include "CameraComponent3D.h"
#include "Transform3D.h"
#include "GpuImageDimensions.h"
#include "GpuMemoryTypes.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

void ShadowMap::Create(const Vector2ui& imageSize) {
	GpuImageSamplerDesc depthSampler{};
	depthSampler.mipMapMode = GpuImageMipmapMode::NONE;
	// depthSampler.addressMode = GpuImageAddressMode::BACKGROUND_WHITE;

	const Vector3ui shadowmapSize = { imageSize.X, imageSize.Y, 1 };
	IGpuMemoryAllocator* memAllocator = Engine::GetRenderer()->GetMemoryAllocator();

	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		unusedColorArrayAttachment[i] = memAllocator->CreateImage(shadowmapSize, GpuImageDimension::d2D, 4, Format::RGBA8_UNORM, GpuImageUsage::COLOR | GpuImageUsage::SAMPLED, GpuSharedMemoryType::GPU_ONLY, 1, depthSampler).GetPointer();
		depthArrayAttachment[i] = memAllocator->CreateImage(shadowmapSize, GpuImageDimension::d2D, 4, Format::D32S8_SFLOAT_SUINT, GpuImageUsage::DEPTH_STENCIL | GpuImageUsage::SAMPLED | GpuImageUsage::SAMPLED_ARRAY, GpuSharedMemoryType::GPU_ONLY, 1, depthSampler).GetPointer();
	}

	shadowsGenMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_shadows.json");
	shadowsGenMaterialInstance = shadowsGenMaterial->CreateInstance().GetPointer();

	const IGpuUniformBuffer* lightUbos[3]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		lightUniformBuffer[i] = Engine::GetRenderer()->GetMemoryAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 4 + sizeof(Vector3f)).GetPointer();
		lightUbos[i] = lightUniformBuffer[i].GetPointer();
	}

	shadowsGenMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLight", lightUbos);
	shadowsGenMaterialInstance->GetSlot("global")->FlushUpdate();
}

void ShadowMap::SetDirectionalLight(const DirectionalLight& dirLight) {
	OSK_ASSERT(shadowsGenMaterial != nullptr, "Se debe crear el ShadowMap antes de poder establecer su luz direccional.");
	OSK_ASSERT(shadowsGenMaterialInstance.HasValue(), "Se debe crear el ShadowMap antes de poder establecer su luz direccional.");

	lightDirection = Vector3f(dirLight.directionAndIntensity.X, dirLight.directionAndIntensity.Y, dirLight.directionAndIntensity.Z);

	if (lightOrigin == Vector3f(0.0f))
		SetLightOrigin(lightDirection * 5.0f);

	UpdateLightMatrixBuffer();
}

void ShadowMap::UpdateLightMatrixBuffer() {
	const TSize uniformBufferIndex = Engine::GetRenderer()->GetCurrentCommandListIndex();

	if (cameraObject == EMPTY_GAME_OBJECT) {
		const glm::mat4 lightMatrix =
			glm::ortho(
				-lightArea.X / 2.0f, lightArea.X / 2.0f,
				-lightArea.Y / 2.0f, lightArea.Y / 2.0f,
				nearPlane, farPlane)
			* glm::lookAt((lightOrigin - lightDirection).ToGLM(), lightOrigin.ToGLM(), glm::vec3(0.0f, 1.0f, 0.0f));

		lightUniformBuffer[uniformBufferIndex]->ResetCursor();
		lightUniformBuffer[uniformBufferIndex]->MapMemory();
		lightUniformBuffer[uniformBufferIndex]->Write(lightMatrix);
		lightUniformBuffer[uniformBufferIndex]->Unmap();

		return;
	}

	ShadowsBufferContent bufferContent{};

	const CameraComponent3D& camera = Engine::GetEntityComponentSystem()->GetComponent<CameraComponent3D>(cameraObject);
	const Transform3D& cameraTransform = Engine::GetEntityComponentSystem()->GetComponent<Transform3D>(cameraObject);

	const float nearClip = camera.GetNearPlane(); /// @todo Poder establecer mínimo y máximo para el clip.
	const float farClip = camera.GetFarPlane(); /// @todo Poder establecer mínimo y máximo para el clip.
	const float cameraClipRange = farClip - nearClip;

	const float minClip = nearClip;
	const float maxClip = nearClip + cameraClipRange;

	const float clipRange = maxClip - minClip;
	const float clipRatio = maxClip / minClip;

	const glm::mat4 cameraProjection = camera.GetProjectionMatrix();
	const glm::mat4 cameraView = camera.GetViewMatrix(cameraTransform);

	float cascadeSplits[numMaps]{ 0.0f };
	for (TSize i = 0; i < numMaps; i++) {
		float p = (i + 1) / static_cast<float>(numMaps);
		float log = minClip * std::pow(clipRatio, p); //clip ratio
		float uniform = minClip + clipRange * p;
		float d = 0.95f * (log - uniform) + uniform;
		cascadeSplits[i] = (d - nearClip) / clipRange;
	}

	bufferContent.cascadeSplits = Vector4f(cascadeSplits[0], cascadeSplits[1], cascadeSplits[2], cascadeSplits[3]);

	float lastClip = 0.0f;
	for (TSize i = 0; i < numMaps; i++) {

		// Esquinas del frustum en world-space.
		DynamicArray<Vector3f> worldSpaceFrustumCorners = GetFrustumCorners(cameraProjection * cameraView);

		for (TSize corner = 0; corner < worldSpaceFrustumCorners.GetSize() / 2; corner++) {
			const Vector3f distanceToCorner = worldSpaceFrustumCorners[corner + 4] - worldSpaceFrustumCorners[corner];

			worldSpaceFrustumCorners[corner + 4] = worldSpaceFrustumCorners[corner] + (distanceToCorner * cascadeSplits[i]);
			worldSpaceFrustumCorners[corner] += (distanceToCorner * lastClip);
		}

		Vector3f frustumCenter = 0.0f;
		for (const auto& corner : worldSpaceFrustumCorners)
			frustumCenter += corner;
		frustumCenter /= static_cast<float>(worldSpaceFrustumCorners.GetSize());

		float radius = 0.0f;
		for (const auto& corner : worldSpaceFrustumCorners)
			radius = glm::max(radius, frustumCenter.GetDistanceTo(corner));
		radius = glm::ceil(radius * 16.0f) / 16.0f;

		const Vector3f maxExtent = radius;
		const Vector3f minExtent = -radius;

		const glm::mat4 lightView = glm::lookAt((frustumCenter - lightDirection.GetNormalized() * radius * 0.5f).ToGLM(), frustumCenter.ToGLM(), glm::vec3(0.0f, 1.0f, 0.0f));
		const glm::mat4 lightProjection = glm::ortho(minExtent.X, maxExtent.X, minExtent.Y, maxExtent.Y, minExtent.Z, maxExtent.Z);

		bufferContent.matrices[i] = lightProjection * lightView;
		lastClip = cascadeSplits[i];

		cascadeSplits[i] = ((nearClip + lastClip * clipRange) * -1.0f);
	}

	bufferContent.cascadeSplits = Vector4f(cascadeSplits[0], cascadeSplits[1], cascadeSplits[2], cascadeSplits[3]);

	lightUniformBuffer[uniformBufferIndex]->ResetCursor();
	lightUniformBuffer[uniformBufferIndex]->MapMemory();
	lightUniformBuffer[uniformBufferIndex]->Write(bufferContent);
	lightUniformBuffer[uniformBufferIndex]->Unmap();
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
	return depthArrayAttachment[index].GetPointer();
}

GpuImage* ShadowMap::GetColorImage(TSize index) const {
	return unusedColorArrayAttachment[index].GetPointer();
}

Material* ShadowMap::GetShadowsMaterial() const {
	return shadowsGenMaterial;
}

MaterialInstance* ShadowMap::GetShadowsMaterialInstance() const {
	return shadowsGenMaterialInstance.GetPointer();
}

DynamicArray<IGpuUniformBuffer*> ShadowMap::GetDirLightMatrixUniformBuffers() const {
	auto output = DynamicArray<IGpuUniformBuffer*>::CreateResizedArray(NUM_RESOURCES_IN_FLIGHT);
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		output[i] = (lightUniformBuffer[i].GetPointer());

	return output;
}

void ShadowMap::SetSceneCamera(ECS::GameObjectIndex cameraObject) {
	this->cameraObject = cameraObject;
}

TSize ShadowMap::GetNumCascades() const {
	return numMaps;
}

DynamicArray<Vector3f> ShadowMap::GetFrustumCorners(const glm::mat4& cameraMatrix) {
	const glm::mat4 inverseMatrix = glm::inverse(cameraMatrix);

	// Normalized Device Coordinates
	DynamicArray<Vector3f> corners = {
		Vector3f(-1.0f,  1.0f, -1.0f),
		Vector3f( 1.0f,  1.0f, -1.0f),
		Vector3f( 1.0f, -1.0f, -1.0f),
		Vector3f(-1.0f, -1.0f, -1.0f),
		Vector3f(-1.0f,  1.0f,  1.0f),
		Vector3f( 1.0f,  1.0f,  1.0f),
		Vector3f( 1.0f, -1.0f,  1.0f),
		Vector3f(-1.0f, -1.0f,  1.0f)
	};

	for (auto& corner : corners) {
		const glm::vec4 inverseResult = inverseMatrix * glm::vec4(corner.ToGLM(), 1.0f);
		corner = glm::vec3(inverseResult / inverseResult.w);
	}

	return corners;
}
