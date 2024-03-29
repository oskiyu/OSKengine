#include "SkyboxRenderSystem.h"

#include "Format.h"
#include "OSKengine.h"
#include "Material.h"
#include "MaterialSystem.h"
#include "CubemapTexture.h"
#include "Model3D.h"

#include "CameraComponent3D.h"
#include "Transform3D.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

SkyboxRenderSystem::SkyboxRenderSystem() {
	_SetSignature({});

	skyboxMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/Skybox/material.json");
	skyboxMaterialInstance = skyboxMaterial->CreateInstance().GetPointer();

	m_cubemapModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/cube.json");

	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> buffers{};
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		cameraUbos[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2).GetPointer();
		buffers[i] = cameraUbos[i].GetPointer();
	}

	skyboxMaterialInstance->GetSlot("camera")->SetUniformBuffers("camera", buffers);
	skyboxMaterialInstance->GetSlot("camera")->FlushUpdate();
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

	skyboxMaterialInstance->GetSlot("texture")->SetGpuImage("skybox", texture->GetGpuImage()->GetView(GpuImageViewConfig::CreateSampled_Cubemap()));
	skyboxMaterialInstance->GetSlot("texture")->FlushUpdate();
}

void SkyboxRenderSystem::Render(ICommandList* commandList) {
	if (cameraObject == EMPTY_GAME_OBJECT)
		return;

	const auto resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	const auto& camera			= Engine::GetEcs()->GetComponent<CameraComponent3D>(cameraObject);
	const auto& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(cameraObject);

	cameraUbos[resourceIndex]->MapMemory();
	cameraUbos[resourceIndex]->Write(camera.GetProjectionMatrix());
	cameraUbos[resourceIndex]->Write(camera.GetViewMatrix(cameraTransform));
	cameraUbos[resourceIndex]->Unmap();

	commandList->StartDebugSection("Skybox Rendering", Color::Red);

	commandList->BeginGraphicsRenderpass(&m_renderTarget, Color::Black * 0.0f);
	this->SetupViewport(commandList);
	commandList->BindMaterial(*skyboxMaterial);
	commandList->BindMaterialInstance(*skyboxMaterialInstance);
	commandList->BindVertexBuffer(*m_cubemapModel->GetVertexBuffer());
	commandList->BindIndexBuffer(*m_cubemapModel->GetIndexBuffer());
	commandList->PushMaterialConstants("brightness", 1.0f);
	commandList->DrawSingleInstance(m_cubemapModel->GetIndexCount());
	commandList->EndGraphicsRenderpass();

	commandList->EndDebugSection();
}
