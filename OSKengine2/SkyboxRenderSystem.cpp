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
	SetSignature({});

	skyboxMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/skybox_material.json");
	skyboxMaterialInstance = skyboxMaterial->CreateInstance().GetPointer();

	cubemapModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/cube.json", "GLOBAL");

	const IGpuUniformBuffer* buffers[3]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		cameraUbos[i] = Engine::GetRenderer()->GetMemoryAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2).GetPointer();
		buffers[i] = cameraUbos[i].GetPointer();
	}

	skyboxMaterialInstance->GetSlot("camera")->SetUniformBuffers("camera", buffers);
	skyboxMaterialInstance->GetSlot("camera")->FlushUpdate();
}

void SkyboxRenderSystem::CreateTargetImage(const Vector2ui& size) {
	renderTarget.Create(size, Format::RGBA32_SFLOAT, Format::D32S8_SFLOAT_SUINT);
	renderTarget.SetName("Skybox Render Target");
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

	const TSize currentFrameIndex = Engine::GetRenderer()->GetCurrentCommandListIndex();

	CameraComponent3D& camera = Engine::GetEntityComponentSystem()->GetComponent<CameraComponent3D>(cameraObject);
	Transform3D& cameraTransform = Engine::GetEntityComponentSystem()->GetComponent<Transform3D>(cameraObject);

	cameraUbos[currentFrameIndex]->MapMemory();
	cameraUbos[currentFrameIndex]->Write(camera.GetProjectionMatrix());
	cameraUbos[currentFrameIndex]->Write(camera.GetViewMatrix(cameraTransform));
	cameraUbos[currentFrameIndex]->Unmap();

	commandList->BeginGraphicsRenderpass(&renderTarget, Color::BLACK() * 0.0f);
	commandList->BindMaterial(skyboxMaterial);
	commandList->BindMaterialSlot(skyboxMaterialInstance->GetSlot("camera"));
	commandList->BindMaterialSlot(skyboxMaterialInstance->GetSlot("texture"));
	commandList->BindVertexBuffer(cubemapModel->GetVertexBuffer());
	commandList->BindIndexBuffer(cubemapModel->GetIndexBuffer());
	commandList->PushMaterialConstants("brightness", 1.0f);
	commandList->DrawSingleInstance(cubemapModel->GetIndexCount());
	commandList->EndGraphicsRenderpass();
}
