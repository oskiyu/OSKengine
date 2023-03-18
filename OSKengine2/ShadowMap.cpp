#include "ShadowMap.h"

#include "Lights.h"

#include "Format.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "Material.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "CameraComponent3D.h"
#include "Transform3D.h"
#include "GpuImageDimensions.h"
#include "GpuMemoryTypes.h"
#include "Model3D.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

void ShadowMap::Create(const Vector2ui& imageSize) {
	GpuImageSamplerDesc depthSampler{};
	depthSampler.mipMapMode = GpuImageMipmapMode::NONE;

	IGpuMemoryAllocator* memAllocator = Engine::GetRenderer()->GetAllocator();

	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		GpuImageCreateInfo imageInfo = GpuImageCreateInfo::CreateDefault2D(
			imageSize, 
			Format::RGBA8_UNORM, 
			GpuImageUsage::COLOR | GpuImageUsage::SAMPLED);

		imageInfo.numLayers = 4;
		imageInfo.samplerDesc = depthSampler;

		unusedColorArrayAttachment[i] = memAllocator->CreateImage(imageInfo).GetPointer();
		unusedColorArrayAttachment[i]->SetDebugName("Shadow Map [" + std::to_string(i) + "] Unused Attachment");

		imageInfo.format = Format::D16_UNORM;
		imageInfo.usage = GpuImageUsage::DEPTH | GpuImageUsage::SAMPLED | GpuImageUsage::SAMPLED_ARRAY;

		depthArrayAttachment[i] = memAllocator->CreateImage(imageInfo).GetPointer();
		depthArrayAttachment[i]->SetDebugName("Shadow Map [" + std::to_string(i) + "] Depth");
	}

	shadowsGenMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/ShadowMapping/material_shadows.json");
	shadowsGenAnimMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/ShadowMapping/material_shadows_anim.json");
	shadowsGenMaterialInstance = shadowsGenMaterial->CreateInstance().GetPointer();

	const IGpuUniformBuffer* lightUbos[3]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		lightUniformBuffer[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 4 + sizeof(Vector3f)).GetPointer();
		lightUbos[i] = lightUniformBuffer[i].GetPointer();
	}

	shadowsGenMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLight", lightUbos);
	shadowsGenMaterialInstance->GetSlot("global")->FlushUpdate();
}

void ShadowMap::SetDirectionalLight(const DirectionalLight& dirLight) {
	OSK_ASSERT(shadowsGenMaterial != nullptr, "Se debe crear el ShadowMap antes de poder establecer su luz direccional.");
	OSK_ASSERT(shadowsGenMaterialInstance.HasValue(), "Se debe crear el ShadowMap antes de poder establecer su luz direccional.");

	lightDirection = Vector3f(dirLight.directionAndIntensity.X, dirLight.directionAndIntensity.Y, dirLight.directionAndIntensity.Z);

	UpdateLightMatrixBuffer();
}

void ShadowMap::UpdateLightMatrixBuffer() {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	ShadowsBufferContent bufferContent{};

	const CameraComponent3D& camera = Engine::GetEcs()->GetComponent<CameraComponent3D>(cameraObject);
	const Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(cameraObject);

	float cascadeSplits[numMaps]{ 1, 5, 15, 50 };
	for (TSize i = 0; i < numMaps; i++)
		cascadeSplits[i] -= camera.GetNearPlane();

	bufferContent.cascadeSplits = Vector4f(cascadeSplits[0], cascadeSplits[1], cascadeSplits[2], cascadeSplits[3]);

	float lastClip = camera.GetNearPlane();

// #define OSK_SHADOWS_NVIDIA
#ifndef OSK_SHADOWS_NVIDIA
#define OSK_SHADOWS_CUSTOM
#endif

	// NVIDIA:
#ifdef OSK_SHADOWS_NVIDIA
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
#endif
#ifdef OSK_SHADOWS_CUSTOM
	
	// CUSTOM:
	for (TSize i = 0; i < numMaps; i++) {

		// Creamos un frustum con NEAR = CascadeSplits[i - 1] y FAR = CascadeSplits[i],
		// de tal manera que quede dentro toda la zona renderizada que está dentro del split.
		CameraComponent3D clipCamera = camera;
		clipCamera.SetPlanes(lastClip, cascadeSplits[i]);
		// Obtiene las esquinas en el espacio del mundo.
		const DynamicArray<Vector3f> worldSpaceFrustumCorners = 
			GetFrustumCorners(clipCamera.GetProjectionMatrix() * clipCamera.GetViewMatrix(cameraTransform));

		// Calculamos el centro del frustum.
		Vector3f frustumCenter = 0.0f;
		for (const auto& corner : worldSpaceFrustumCorners)
			frustumCenter += corner;
		frustumCenter /= static_cast<float>(worldSpaceFrustumCorners.GetSize());

		// Movemos para que el centro este en Y = 0.
		for (auto& corner : worldSpaceFrustumCorners)
			corner += lightDirection * frustumCenter.Y;
		frustumCenter += lightDirection * frustumCenter.Y;

		// Calculamos el radio, que será el radio del frustum que renderizará el mapa de sombras.
		float radius = 0.0f;
		for (const auto& corner : worldSpaceFrustumCorners)
			radius = glm::max(radius, frustumCenter.GetDistanceTo(corner));
		radius = glm::ceil(radius * 16.0f) / 16.0f;

		// Obtenemos alturas min y max, para poder establecer correctamente los
		// planos near y far del frustum final.
		const float yPerUnit = 1.0f / glm::radians(90.0f - lightDirection.GetAngle({ 0.0f, -1.0f, 0.0f }));

		const float minCornerHeight = -yPerUnit * radius;
		const float maxCornerHeight = yPerUnit * radius;

		const float shadowNearClip = (minCornerHeight) / yPerUnit;
		const float shadowFarClip = maxCornerHeight / yPerUnit;

		// Extents del frustum
		const Vector2f maxExtent = radius;
		const Vector2f minExtent = -maxExtent;

		// 'Cámara' virtual para renderizar el mapa de sombras.
		const glm::mat4 lightProjection = glm::ortho(minExtent.X, maxExtent.X, maxExtent.Y, minExtent.Y, nearPlane * (i + 1), farPlane * (i + 1));
		const glm::mat4 lightView = glm::lookAt((frustumCenter - lightDirection).ToGLM(), frustumCenter.ToGLM(), glm::vec3(0.0f, 1.0f, 0.0f));

		bufferContent.matrices[i] = lightProjection * lightView;

		lastClip = cascadeSplits[i];
	}
#endif

	lightUniformBuffer[resourceIndex]->ResetCursor();
	lightUniformBuffer[resourceIndex]->MapMemory();
	lightUniformBuffer[resourceIndex]->Write(bufferContent.matrices[0]);
	lightUniformBuffer[resourceIndex]->Write(bufferContent.matrices[1]);
	lightUniformBuffer[resourceIndex]->Write(bufferContent.matrices[2]);
	lightUniformBuffer[resourceIndex]->Write(bufferContent.matrices[3]);
	lightUniformBuffer[resourceIndex]->Write(bufferContent.cascadeSplits);
	lightUniformBuffer[resourceIndex]->Unmap();
}

void ShadowMap::SetNearPlane(float nearPlane) {
	this->nearPlane = nearPlane;
}

void ShadowMap::SetFarPlane(float farPlane) {
	this->farPlane = farPlane;
}

GpuImage* ShadowMap::GetShadowImage(TSize index) const {
	return depthArrayAttachment[index].GetPointer();
}

GpuImage* ShadowMap::GetColorImage(TSize index) const {
	return unusedColorArrayAttachment[index].GetPointer();
}

Material* ShadowMap::GetShadowsMaterial(ModelType modelType) const {
	if (modelType == ModelType::STATIC_MESH)
		return shadowsGenMaterial;
	else
		return shadowsGenAnimMaterial;
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
		Vector3f(-1.0f,  1.0f,  1.0f),
		Vector3f( 1.0f,  1.0f,  1.0f),
		Vector3f( 1.0f, -1.0f, -1.0f),
		Vector3f(-1.0f, -1.0f, -1.0f),
		Vector3f( 1.0f, -1.0f,  1.0f),
		Vector3f(-1.0f, -1.0f,  1.0f),
	};

	for (auto& corner : corners) {
		const glm::vec4 inverseResult = inverseMatrix * glm::vec4(corner.ToGLM(), 1.0f);
		corner = glm::vec3(inverseResult / inverseResult.w);
	}

	return corners;
}
