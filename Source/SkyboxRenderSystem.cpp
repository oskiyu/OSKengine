#include "SkyboxRenderSystem.h"

#include "Format.h"
#include "OSKengine.h"
#include "Material.h"
#include "MaterialSystem.h"
#include "CubemapTexture.h"
#include "Model3D.h"

#include "CameraComponent3D.h"
#include "Transform3D.h"
#include <span>
#include "GameObject.h"
#include "ICommandList.h"
#include "ResourcesInFlight.h"
#include "AssetRef.h"
#include "NumericTypes.h"
#include "Vector2.hpp"
#include "Uuid.h"
#include "AssetManager.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

SkyboxRenderSystem::SkyboxRenderSystem() {
	_SetSignature({});

	m_skyboxMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial(MaterialName);

	for (auto& mInstance : m_skyboxMaterialInstances) {
		mInstance = m_skyboxMaterial->CreateInstance().GetPointer();
	}

	m_cubemapModel = Engine::GetAssetManager()->Load<Model3D>(CubeModelPath);

	for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		m_cameraUbos[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2).GetPointer();
		
		m_skyboxMaterialInstances[i]->GetSlot("camera")->SetUniformBuffer("camera", m_cameraUbos[i].GetValue());
		m_skyboxMaterialInstances[i]->GetSlot("camera")->FlushUpdate();
	}
}

void SkyboxRenderSystem::CreateTargetImage(const Vector2ui& size) {
	RenderTargetAttachmentInfo colorInfo{ .format = Format::RGBA16_SFLOAT , .name = "Skybox Render Target" };
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D16_UNORM , .name = "Skybox Depth" };
	m_renderTarget.Create(size, { colorInfo }, depthInfo);
}

void SkyboxRenderSystem::SetCamera(GameObjectIndex cameraObject) {
	this->cameraObject = cameraObject;
}

void SkyboxRenderSystem::SetCubemap(ASSETS::AssetRef<ASSETS::CubemapTexture> texture) {
	m_skybox = texture;

	for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		m_skyboxMaterialInstances[i]->GetSlot("texture")->SetGpuImage("skybox", *texture->GetGpuImage()->GetView(GpuImageViewConfig::CreateSampled_Cubemap()), GpuImageSamplerDesc::CreateDefault_NoMipMap());
		m_skyboxMaterialInstances[i]->GetSlot("texture")->FlushUpdate();
	}
}

void SkyboxRenderSystem::Render(ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) {
	if (cameraObject == EMPTY_GAME_OBJECT)
		return;

	const auto resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	const auto& camera			= Engine::GetEcs()->GetComponent<CameraComponent3D>(cameraObject);
	const auto& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(cameraObject);

	m_cameraUbos[resourceIndex]->MapMemory();
	m_cameraUbos[resourceIndex]->Write(camera.GetProjectionMatrix());
	m_cameraUbos[resourceIndex]->Write(camera.GetViewMatrix(cameraTransform));
	m_cameraUbos[resourceIndex]->Unmap();

	commandList->StartDebugSection("Skybox Rendering", Color::Red);

	commandList->BeginGraphicsRenderpass(&m_renderTarget, Color::Black * 0.0f);
	this->SetupViewport(commandList);
	commandList->BindMaterial(*m_skyboxMaterial);
	commandList->BindMaterialInstance(*m_skyboxMaterialInstances[Engine::GetRenderer()->GetCurrentResourceIndex()]);
	commandList->BindVertexBuffer(m_cubemapModel->GetModel().GetVertexBuffer());
	commandList->BindIndexBuffer(m_cubemapModel->GetModel().GetIndexBuffer());
	commandList->PushMaterialConstants("brightness", 1.0f);
	commandList->DrawSingleInstance(m_cubemapModel->GetModel().GetTotalIndexCount());
	commandList->EndGraphicsRenderpass();

	commandList->EndDebugSection();
}

const CubemapTexture* SkyboxRenderSystem::GetCurrentTexture() const {
	return m_skybox.GetAsset();
}

GameObjectIndex SkyboxRenderSystem::GetCurrentCameraObject() const {
	return cameraObject;
}

const Material* SkyboxRenderSystem::GetCurrentMaterial() const {
	return m_skyboxMaterial;
}


nlohmann::json SkyboxRenderSystem::SaveConfiguration() const {
	auto output = nlohmann::json();

	output["cameraObject"] = cameraObject.Get();

	// m_skybox y m_cubemapModel se cargan al crear el sistema:
	// no es necesario serializarlo.

	return output;
}

PERSISTENCE::BinaryBlock SkyboxRenderSystem::SaveBinaryConfiguration() const {
	auto output = PERSISTENCE::BinaryBlock::Empty();

	output.Write(cameraObject.Get());

	return output;
}

void SkyboxRenderSystem::ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) {
	cameraObject = translator.GetCurrentIndex(GameObjectIndex(config["cameraObject"]));
	UpdatePassesCamera(cameraObject);
	// m_skybox y m_cubemapModel se cargan al crear el sistema:
	// no es necesario cargarlo de nuevo.
}

void SkyboxRenderSystem::ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) {
	cameraObject = translator.GetCurrentIndex(GameObjectIndex(reader->Read<GameObjectIndex::TUnderlyingType>()));
	UpdatePassesCamera(cameraObject);
	// cubeModel y sphereModel se cargan al crear el sistema:
	// no es necesario cargarlo de nuevo.
}
