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

#include "InvalidObjectStateException.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

void ShadowMap::Create(const Vector2ui& imageSize) {
	GpuImageSamplerDesc depthSampler{};
	depthSampler.mipMapMode = GpuImageMipmapMode::NONE;
	depthSampler.addressMode = GpuImageAddressMode::REPEAT;

	IGpuMemoryAllocator* memAllocator = Engine::GetRenderer()->GetAllocator();

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		GpuImageCreateInfo imageInfo = GpuImageCreateInfo::CreateDefault2D(
			imageSize, 
			Format::RGBA8_UNORM, 
			GpuImageUsage::COLOR | GpuImageUsage::SAMPLED);

		imageInfo.numLayers = 4;
		imageInfo.samplerDesc = depthSampler;

		unusedColorArrayAttachment[i] = memAllocator->CreateImage(imageInfo).GetPointer();
		unusedColorArrayAttachment[i]->SetDebugName(std::format("Shadow Map[{}] Unused", i));

		imageInfo.format = Format::D32_SFLOAT;
		imageInfo.usage = GpuImageUsage::DEPTH | GpuImageUsage::SAMPLED | GpuImageUsage::SAMPLED_ARRAY;

		depthArrayAttachment[i] = memAllocator->CreateImage(imageInfo).GetPointer();
		depthArrayAttachment[i]->SetDebugName(std::format("Shadow Map[{}] Depth", i));
	}

	shadowsGenMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/ShadowMapping/material_shadows.json");
	shadowsGenAnimMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/ShadowMapping/material_shadows_anim.json");
	shadowsGenMaterialInstance = shadowsGenMaterial->CreateInstance().GetPointer();

	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> lightUbos{};
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		lightUniformBuffer[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 4 + sizeof(Vector4f)).GetPointer();
		lightUbos[i] = lightUniformBuffer[i].GetPointer();
	}

	shadowsGenMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLight", lightUbos);
	shadowsGenMaterialInstance->GetSlot("global")->FlushUpdate();
}

void ShadowMap::SetDirectionalLight(const DirectionalLight& dirLight) {
	OSK_ASSERT(shadowsGenMaterial != nullptr, InvalidObjectStateException("Se debe crear el ShadowMap antes de poder establecer su luz direccional."));
	OSK_ASSERT(shadowsGenMaterialInstance.HasValue(), InvalidObjectStateException("Se debe crear el ShadowMap antes de poder establecer su luz direccional."));

	lightDirection = Vector3f(dirLight.directionAndIntensity.x, dirLight.directionAndIntensity.y, dirLight.directionAndIntensity.Z).GetNormalized();

	UpdateLightMatrixBuffer();
}

void ShadowMap::UpdateLightMatrixBuffer() {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	ShadowsBufferContent bufferContent{};

	const CameraComponent3D& camera = Engine::GetEcs()->GetComponent<CameraComponent3D>(cameraObject);
	const Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(cameraObject);

	float cascadeSplits[numMaps]{ 3, 13, 25, 50 };

	bufferContent.cascadeSplits = Vector4f(cascadeSplits[0], cascadeSplits[1], cascadeSplits[2], cascadeSplits[3]);

	float lastClip = camera.GetNearPlane();

	// Matriz vista de la cámara del jugador.
	const glm::mat4 viewMatrix = camera.GetViewMatrix(cameraTransform);

	for (UIndex32 i = 0; i < numMaps; i++) {

		// Creamos un frustum con NEAR = CascadeSplits[i - 1] y FAR = CascadeSplits[i],
		// de tal manera que quede dentro toda la zona renderizada que está dentro del split.
		CameraComponent3D clipCamera = camera;
		clipCamera.SetPlanes(lastClip, cascadeSplits[i]);

		// Matriz proyección de la cámara del jugadro, en la que los planos near y far
		// han sido recortados para representar el nivel de shadowmap.
		const glm::mat4 croppedProjection = clipCamera.GetProjectionMatrix();

		// Obtiene las esquinas en el espacio del mundo.
		DynamicArray<Vector3f> worldSpaceFrustumCorners = 
			GetFrustumCorners(croppedProjection * viewMatrix);

		// Calculamos el centro del frustum.
		Vector3f frustumCenter = 0.0f;
		for (const auto& corner : worldSpaceFrustumCorners)
			frustumCenter += corner;
		frustumCenter /= static_cast<float>(worldSpaceFrustumCorners.GetSize());

		// Movemos para que el centro este en Y = 0.
		for (auto& corner : worldSpaceFrustumCorners)
			corner += lightDirection * frustumCenter.y;
		frustumCenter += lightDirection * frustumCenter.y;

		// Calculamos el radio, que será el radio del frustum que renderizará el mapa de sombras.
		float radius = 0.0f;
		for (const auto& corner : worldSpaceFrustumCorners)
			radius = glm::max(radius, frustumCenter.GetDistanceTo(corner));
		radius = glm::ceil(radius * 16.0f) / 16.0f;

		// Extents del frustum
		const Vector2f maxExtent = Vector2f(radius * 1.2f);
		const Vector2f minExtent = -maxExtent;

		const float minY = minExtent.y * lightDirection.GetNormalized().y;
		const float maxY = maxExtent.y * lightDirection.GetNormalized().y;

		// 'Cámara' virtual para renderizar el mapa de sombras.
		const glm::mat4 lightProjection = glm::ortho(minExtent.x, maxExtent.x, maxExtent.y, minExtent.y, nearPlane * 12, farPlane * 4);
		const glm::mat4 lightView = glm::lookAt((frustumCenter - lightDirection).ToGLM(), frustumCenter.ToGLM(), glm::vec3(0.0f, 1.0f, 0.0f));

		bufferContent.matrices[i] = lightProjection * lightView;

		lastClip = cascadeSplits[i];
	}

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

GpuImage* ShadowMap::GetShadowImage(UIndex32 index) {
	return depthArrayAttachment[index].GetPointer();
}

GpuImage* ShadowMap::GetColorImage(UIndex32 index) {
	return unusedColorArrayAttachment[index].GetPointer();
}

Material* ShadowMap::GetShadowsMaterial(ModelType modelType) {
	if (modelType == ModelType::STATIC_MESH)
		return shadowsGenMaterial;
	else
		return shadowsGenAnimMaterial;
}

MaterialInstance* ShadowMap::GetShadowsMaterialInstance() {
	return shadowsGenMaterialInstance.GetPointer();
}

DynamicArray<GpuBuffer*> ShadowMap::GetDirLightMatrixUniformBuffers() {
	auto output = DynamicArray<GpuBuffer*>::CreateResizedArray(NUM_RESOURCES_IN_FLIGHT);
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		output[i] = (lightUniformBuffer[i].GetPointer());

	return output;
}

void ShadowMap::SetSceneCamera(ECS::GameObjectIndex cameraObject) {
	this->cameraObject = cameraObject;
}

UIndex32 ShadowMap::GetNumCascades() const {
	return numMaps;
}

DynamicArray<Vector3f> ShadowMap::GetFrustumCorners(const glm::mat4& cameraMatrix) {
	const glm::mat4 inverseMatrix = glm::inverse(cameraMatrix);

	// Normalized Device Coordinates
	DynamicArray<Vector3f> corners = {
		Vector3f( 1,  1,  1),
		Vector3f( 1,  1, -1),
		Vector3f(-1,  1,  1),
		Vector3f(-1,  1, -1),
		Vector3f( 1, -1,  1),
		Vector3f( 1, -1, -1),
		Vector3f(-1, -1,  1),
		Vector3f(-1, -1, -1),
	};

	for (auto& corner : corners) {
		const glm::vec4 inverseResult = inverseMatrix * glm::vec4(corner.ToGLM(), 1.0f);
		corner = glm::vec3(inverseResult / inverseResult.w);
	}

	return corners;
}
