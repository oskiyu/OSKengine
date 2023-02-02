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

	skyboxMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/skybox_material.json");
	skyboxMaterialInstance = skyboxMaterial->CreateInstance().GetPointer();

	cubemapModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/cube.json", "GLOBAL");

	const IGpuUniformBuffer* buffers[3]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		cameraUbos[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2).GetPointer();
		buffers[i] = cameraUbos[i].GetPointer();
	}

	skyboxMaterialInstance->GetSlot("camera")->SetUniformBuffers("camera", buffers);
	skyboxMaterialInstance->GetSlot("camera")->FlushUpdate();
}

void SkyboxRenderSystem::CreateTargetImage(const Vector2ui& size) {
	RenderTargetAttachmentInfo colorInfo{ .format = Format::RGBA16_SFLOAT , .name = "Skybox Render Target" };
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D32S8_SFLOAT_SUINT , .name = "Skybox Depth" };
	renderTarget.Create(size, { colorInfo }, depthInfo);
}

void SkyboxRenderSystem::SetCamera(GameObjectIndex cameraObject) {
	this->cameraObject = cameraObject;
}

void SkyboxRenderSystem::SetCubemap(const CubemapTexture& texture) {
	skyboxMaterialInstance->GetSlot("texture")->SetGpuImage("skybox", texture.GetGpuImage());
	skyboxMaterialInstance->GetSlot("texture")->FlushUpdate();
}

void SkyboxRenderSystem::Render(ICommandList* commandList) {
	if (cameraObject == EMPTY_GAME_OBJECT)
		return;

	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	CameraComponent3D& camera = Engine::GetEcs()->GetComponent<CameraComponent3D>(cameraObject);
	Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(cameraObject);

	cameraUbos[resourceIndex]->MapMemory();
	cameraUbos[resourceIndex]->Write(camera.GetProjectionMatrix());
	cameraUbos[resourceIndex]->Write(camera.GetViewMatrix(cameraTransform));
	cameraUbos[resourceIndex]->Unmap();

	commandList->StartDebugSection("Skybox Rendering", Color::RED());

	commandList->BeginGraphicsRenderpass(&renderTarget, Color::BLACK() * 0.0f);
	commandList->BindMaterial(skyboxMaterial);
	commandList->BindMaterialSlot(skyboxMaterialInstance->GetSlot("camera"));
	commandList->BindMaterialSlot(skyboxMaterialInstance->GetSlot("texture"));
	commandList->BindVertexBuffer(cubemapModel->GetVertexBuffer());
	commandList->BindIndexBuffer(cubemapModel->GetIndexBuffer());
	commandList->PushMaterialConstants("brightness", 1.0f);
	commandList->DrawSingleInstance(cubemapModel->GetIndexCount());
	commandList->EndGraphicsRenderpass();

	commandList->EndDebugSection();
}
