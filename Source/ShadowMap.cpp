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
	depthSampler.addressMode = GpuImageAddressMode::BACKGROUND_WHITE;

	IGpuMemoryAllocator* memAllocator = Engine::GetRenderer()->GetAllocator();

	GpuImageCreateInfo imageInfo = GpuImageCreateInfo::CreateDefault2D(
		imageSize,
		Format::RGBA8_UNORM,
		GpuImageUsage::COLOR | GpuImageUsage::SAMPLED);

	imageInfo.numLayers = 4;
	imageInfo.samplerDesc = depthSampler;

	m_unusedColorArrayAttachment = memAllocator->CreateImage(imageInfo).GetPointer();
	m_unusedColorArrayAttachment->SetDebugName("Shadow Map Unused");

	imageInfo.format = Format::D32_SFLOAT;
	imageInfo.usage = GpuImageUsage::DEPTH | GpuImageUsage::SAMPLED | GpuImageUsage::SAMPLED_ARRAY;

	m_depthArrayAttachment = memAllocator->CreateImage(imageInfo).GetPointer();
	m_depthArrayAttachment->SetDebugName("Shadow Map Depth");

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		lightUniformBuffer[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 4 + sizeof(Vector4f)).GetPointer();
	}
}

void ShadowMap::SetDirectionalLight(const DirectionalLight& dirLight) {
	// OSK_ASSERT(m_shadowsGenMaterial != nullptr, InvalidObjectStateException("Se debe crear el ShadowMap antes de poder establecer su luz direccional."));
	// OSK_ASSERT(m_shadowsGenMaterialInstance.HasValue(), InvalidObjectStateException("Se debe crear el ShadowMap antes de poder establecer su luz direccional."));

	m_lightDirection = 
		Vector3f(dirLight.directionAndIntensity.x, dirLight.directionAndIntensity.y, dirLight.directionAndIntensity.Z).GetNormalized();

	UpdateLightMatrixBuffer();
}

void ShadowMap::UpdateLightMatrixBuffer() {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	ShadowsBufferContent bufferContent{};

	const CameraComponent3D& camera = Engine::GetEcs()->GetComponent<CameraComponent3D>(m_cameraObject);
	const Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(m_cameraObject);

	bufferContent.cascadeSplits = m_splitsVec;

	float lastClip = camera.GetNearPlane();

	// Matriz vista de la cámara del jugador.
	const glm::mat4 viewMatrix = camera.GetViewMatrix(cameraTransform);

	for (UIndex32 i = 0; i < numMaps; i++) {

		// Creamos un frustum con NEAR = CascadeSplits[i - 1] y FAR = CascadeSplits[i],
		// de tal manera que quede dentro toda la zona renderizada que está dentro del split.
		CameraComponent3D clipCamera = camera;
		clipCamera.SetPlanes(lastClip, m_splits[i]);

		// Matriz proyección de la cámara del jugadro, en la que los planos near y far
		// han sido recortados para representar el nivel de shadowmap.
		const glm::mat4 croppedProjection = clipCamera.GetProjectionMatrix_UnreversedZ();

		// Obtiene las esquinas en el espacio del mundo.
		DynamicArray<Vector3f> worldSpaceFrustumCorners = 
			GetFrustumCorners(croppedProjection * viewMatrix);

		// Calculamos el centro del frustum.
		Vector3f frustumCenter = Vector3f::Zero;
		for (const auto& corner : worldSpaceFrustumCorners)
			frustumCenter += corner;
		frustumCenter /= static_cast<float>(worldSpaceFrustumCorners.GetSize());

		// Movemos para que el centro este en Y = 0.
		for (auto& corner : worldSpaceFrustumCorners)
			corner += m_lightDirection * frustumCenter.y;
		frustumCenter += m_lightDirection * frustumCenter.y;

		// Calculamos el radio, que será el radio del frustum que renderizará el mapa de sombras.
		float radius = 0.0f;
		for (const auto& corner : worldSpaceFrustumCorners)
			radius = glm::max(radius, frustumCenter.GetDistanceTo(corner));
		radius = glm::ceil(radius * 16.0f) / 16.0f;

		// Extents del frustum
		const Vector2f maxExtent = Vector2f(radius * 1.2f);
		const Vector2f minExtent = -maxExtent;

		// 'Cámara' virtual para renderizar el mapa de sombras.
		const glm::mat4 lightProjection = glm::orthoRH_ZO(minExtent.x, maxExtent.x, maxExtent.y, minExtent.y, m_farPlane * 6, m_nearPlane * 12);
		const glm::mat4 lightView = glm::lookAt((frustumCenter - m_lightDirection).ToGlm(), frustumCenter.ToGlm(), glm::vec3(0.0f, 1.0f, 0.0f));

		bufferContent.matrices[i] = lightProjection * lightView;

		lastClip = m_splits[i];
	}

	lightUniformBuffer[resourceIndex]->ResetCursor();
	lightUniformBuffer[resourceIndex]->MapMemory();
	lightUniformBuffer[resourceIndex]->Write(bufferContent);
	lightUniformBuffer[resourceIndex]->Unmap();
}

void ShadowMap::SetNearPlane(float nearPlane) {
	m_nearPlane = nearPlane;
}

void ShadowMap::SetFarPlane(float farPlane) {
	m_farPlane = farPlane;
}

GpuImage* ShadowMap::GetShadowImage() {
	return m_depthArrayAttachment.GetPointer();
}

GpuImage* ShadowMap::GetColorImage() {
	return m_unusedColorArrayAttachment.GetPointer();
}

DynamicArray<GpuBuffer*> ShadowMap::GetDirLightMatrixUniformBuffers() {
	auto output = DynamicArray<GpuBuffer*>::CreateResizedArray(NUM_RESOURCES_IN_FLIGHT);
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		output[i] = (lightUniformBuffer[i].GetPointer());

	return output;
}

void ShadowMap::SetSceneCamera(ECS::GameObjectIndex cameraObject) {
	m_cameraObject = cameraObject;
}

UIndex32 ShadowMap::GetNumCascades() const {
	return numMaps;
}

void ShadowMap::SetSplits(const std::array<float, 4>& splits) {
	m_splits = splits;
	m_splitsVec = Vector4f(splits[0], splits[1], splits[2], splits[3]);
}

std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> ShadowMap::GetGpuBuffers() const {
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> output{};

	for (UIndex64 i = 0; i < output.size(); i++) {
		output[i] = lightUniformBuffer[i].GetPointer();
	}

	return output;
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
		const glm::vec4 inverseResult = inverseMatrix * glm::vec4(corner.ToGlm(), 1.0f);
		corner = Vector3f(glm::vec3(inverseResult / inverseResult.w));
	}

	return corners;
}
