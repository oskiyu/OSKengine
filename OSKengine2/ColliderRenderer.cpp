// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#include "ColliderRenderer.h"

#include "OSKengine.h"
#include "AssetManager.h"
#include "Model3D.h"
#include "Material.h"

#include "Transform3D.h"
#include "Collider.h"
#include "CameraComponent3D.h"

#include "AxisAlignedBoundingBox.h"
#include "SphereCollider.h"
#include "ConvexVolume.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;
using namespace OSK::COLLISION;

ColliderRenderSystem::ColliderRenderSystem() {
	Signature signature{};
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Collider>());
	_SetSignature(signature);

	// Material load
	material = Engine::GetRenderer()->GetMaterialSystem()
		->LoadMaterial("Resources/Materials/Collision/collision_material.json");
	materialInstance = material->CreateInstance().GetPointer();

	// Asset load
	cubeModel = Engine::GetAssetManager()->Load<Model3D>("", "ColliderRenderSystem");
	sphereModel = Engine::GetAssetManager()->Load<Model3D>("", "ColliderRenderSystem");

	// Material setup
	const IGpuUniformBuffer* _cameraUbos[NUM_RESOURCES_IN_FLIGHT]{};
	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		cameraUbos[i] = Engine::GetRenderer()->GetAllocator()
			->CreateUniformBuffer(sizeof(glm::mat4) * 2 + sizeof(glm::vec4))
			.GetPointer();
		_cameraUbos[i] = cameraUbos[i].GetPointer();
	}

	materialInstance->GetSlot("global")->SetUniformBuffers("Camera", _cameraUbos);
	materialInstance->GetSlot("global")->FlushUpdate();
}

void ColliderRenderSystem::Initialize(GameObjectIndex camera) {
	this->cameraObject = camera;
}

void ColliderRenderSystem::CreateTargetImage(const Vector2ui& size) {
	RenderTargetAttachmentInfo colorInfo{ .format = Format::RGBA32_SFLOAT, .name = "Collider Render System Target" };
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D32S8_SFLOAT_SUINT, .name = "Collider Render System Depth" };
	renderTarget.Create(size, { colorInfo }, depthInfo);
}

void ColliderRenderSystem::Render(GRAPHICS::ICommandList* commandList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	const CameraComponent3D& camera = Engine::GetEcs()->GetComponent<CameraComponent3D>(cameraObject);
	const Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(cameraObject);

	cameraUbos[resourceIndex]->MapMemory();
	cameraUbos[resourceIndex]->Write(camera.GetProjectionMatrix());
	cameraUbos[resourceIndex]->Write(camera.GetViewMatrix(cameraTransform));
	cameraUbos[resourceIndex]->Write(cameraTransform.GetPosition());
	cameraUbos[resourceIndex]->Unmap();

	// Render
	struct RenderInfo {
		glm::mat4 modelMatrix;
		Color color;
	} renderInfo;

	commandList->SetGpuImageBarrier(renderTarget.GetMainColorImage(resourceIndex), 
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), 
		GpuBarrierInfo(GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT, GpuBarrierAccessStage::COLOR_ATTACHMENT_WRITE));

	commandList->BeginGraphicsRenderpass(&renderTarget, Color::BLACK() * 0.0f);
	SetupViewport(commandList);

	commandList->BindMaterial(material);
	commandList->BindMaterialSlot(materialInstance->GetSlot("global"));

	for (GameObjectIndex gameObject : GetObjects()) {
		Transform3D transform = Engine::GetEcs()->GetComponent<Transform3D>(gameObject);
		// transform.SetRotation(Quaternion::CreateFromEulerAngles(0.0f));

		const Collider& collider = Engine::GetEcs()->GetComponent<Collider>(gameObject);
		const ITopLevelCollider* topLevelCollider = collider.GetTopLevelCollider();

		renderInfo.color = Color::RED();

		commandList->PushMaterialConstants("Model", renderInfo);

		if (auto* box = reinterpret_cast<const AxisAlignedBoundingBox*>(topLevelCollider)) {
			transform.SetScale(box->GetSize());
			renderInfo.modelMatrix = transform.GetAsMatrix();

			commandList->BindVertexBuffer(cubeModel->GetVertexBuffer());
			commandList->BindIndexBuffer(cubeModel->GetIndexBuffer());
			commandList->DrawSingleInstance(cubeModel->GetIndexCount());
		} else
		if (auto* sphere = reinterpret_cast<const SphereCollider*>(topLevelCollider)) {
			transform.SetScale(sphere->GetRadius());
			renderInfo.modelMatrix = transform.GetAsMatrix();

			commandList->BindVertexBuffer(sphereModel->GetVertexBuffer());
			commandList->BindIndexBuffer(sphereModel->GetIndexBuffer());
			commandList->DrawSingleInstance(sphereModel->GetIndexCount());
		}

		for (TIndex i = 0; i < collider.GetBottomLevelCollidersCount(); i++) {
			// TODO
			
			// const IBottomLevelCollider* bottomLevelCollider = collider.GetBottomLevelCollider(i);
			// const ConvexVolume*;
		}
	}

	commandList->EndGraphicsRenderpass();
}
