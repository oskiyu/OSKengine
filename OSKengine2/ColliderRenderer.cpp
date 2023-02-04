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

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "CollisionEvent.h"
#include "Vertex3D.h"

#include "IGpuMemoryAllocator.h"

#include <set>

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
	lowLevelMaterial = Engine::GetRenderer()->GetMaterialSystem()
		->LoadMaterial("Resources/Materials/Collision/lowlevel_collision_material.json");
	materialInstance = material->CreateInstance().GetPointer();

	// Asset load
	cubeModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/Colliders/cube.json", "ColliderRenderSystem");
	sphereModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/Colliders/sphere.json", "ColliderRenderSystem");

	// Material setup
	const IGpuUniformBuffer* _cameraUbos[NUM_RESOURCES_IN_FLIGHT]{};
	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		cameraUbos[i] = Engine::GetRenderer()->GetAllocator()
			->CreateUniformBuffer(sizeof(glm::mat4) * 2 + sizeof(glm::vec4))
			.GetPointer();
		_cameraUbos[i] = cameraUbos[i].GetPointer();
	}

	materialInstance->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
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
	if (cameraObject == EMPTY_GAME_OBJECT)
		return;

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

	// "polygon_mode": "LINE",

	commandList->BeginGraphicsRenderpass(&renderTarget, Color::BLACK() * 0.0f);
	SetupViewport(commandList);

	commandList->StartDebugSection("Collision Render", Color::RED());


	const auto& eventQueue = Engine::GetEcs()->GetEventQueue<CollisionEvent>();
	std::set<GameObjectIndex> collisionObjects;
	for (const CollisionEvent ev : eventQueue) {
		collisionObjects.insert(ev.firstEntity);
		collisionObjects.insert(ev.secondEntity);
	}

	for (GameObjectIndex gameObject : GetObjects()) {
		commandList->BindMaterial(material);
		commandList->BindMaterialSlot(materialInstance->GetSlot("global"));

		const Transform3D& originalTransform = Engine::GetEcs()->GetComponent<Transform3D>(gameObject);
		Transform3D topLevelTransform = originalTransform;
		topLevelTransform.SetRotation({});

		const Collider& collider = Engine::GetEcs()->GetComponent<Collider>(gameObject);
		const ITopLevelCollider* topLevelCollider = collider.GetTopLevelCollider();

		if (collisionObjects.contains(gameObject))
			renderInfo.color = Color::RED() * 0.75f;
		else
			renderInfo.color = Color::YELLOW() * 0.75f;

		if (auto* box = dynamic_cast<const AxisAlignedBoundingBox*>(topLevelCollider)) {
			topLevelTransform.SetScale(box->GetSize());
			renderInfo.modelMatrix = topLevelTransform.GetAsMatrix();
			commandList->PushMaterialConstants("pushConstants", renderInfo);

			commandList->BindVertexBuffer(cubeModel->GetVertexBuffer());
			commandList->BindIndexBuffer(cubeModel->GetIndexBuffer());
			commandList->DrawSingleInstance(cubeModel->GetIndexCount());
		} else
		if (auto* sphere = dynamic_cast<const SphereCollider*>(topLevelCollider)) {
			topLevelTransform.SetScale(sphere->GetRadius());
			renderInfo.modelMatrix = topLevelTransform.GetAsMatrix();
			commandList->PushMaterialConstants("pushConstants", renderInfo);

			commandList->BindVertexBuffer(sphereModel->GetVertexBuffer());
			commandList->BindIndexBuffer(sphereModel->GetIndexBuffer());
			commandList->DrawSingleInstance(sphereModel->GetIndexCount());
		}

		if (bottomLevelVertexBuffers.ContainsKey(gameObject)) {
			const auto& vertexBuffers = bottomLevelVertexBuffers.Get(gameObject);
			const auto& indexBuffers = bottomLevelIndexBuffers.Get(gameObject);

			if (collisionObjects.contains(gameObject))
				renderInfo.color = Color::RED();
			else
				renderInfo.color = Color::YELLOW();

			renderInfo.modelMatrix = originalTransform.GetAsMatrix();
			commandList->PushMaterialConstants("pushConstants", renderInfo);

			commandList->BindMaterial(lowLevelMaterial);
			for (TIndex i = 0; i < vertexBuffers.GetSize(); i++) {
				commandList->BindVertexBuffer(vertexBuffers[i].GetPointer());
				commandList->BindIndexBuffer(indexBuffers[i].GetPointer());
				commandList->DrawSingleInstance(indexBuffers[i]->GetNumIndices());
			}
		}
	}

	commandList->EndDebugSection();

	commandList->EndGraphicsRenderpass();
}

void ColliderRenderSystem::SetupBottomLevelModel(GameObjectIndex obj) {
	const Collider& collider = Engine::GetEcs()->GetComponent<Collider>(obj);

	DynamicArray<OwnedPtr<IGpuVertexBuffer>> vertexBuffers;
	DynamicArray<OwnedPtr<IGpuIndexBuffer>> indexBuffers;

	if (bottomLevelVertexBuffers.ContainsKey(obj)) {
		bottomLevelVertexBuffers.Get(obj).Empty();
		bottomLevelIndexBuffers.Get(obj).Empty();
	}

	for (TIndex c = 0; c < collider.GetBottomLevelCollidersCount(); c++) {
		const auto& blc = *collider.GetBottomLevelCollider(c)->As<ConvexVolume>();

		DynamicArray<Vertex3D> vertices;
		for (const auto& cVertex : blc.GetLocalSpaceVertices()) {
			vertices.Insert(Vertex3D{
				.position = cVertex,
				.normal = 0.0f,
				.color = Color::WHITE(),
				.texCoords = 0.0f
				});
		}

		DynamicArray<TIndexSize> indices;
		for (const auto& face : blc.GetFaceIndices()) {
			for (const auto index : face)
				indices.Insert(index);

			indices.Insert(face[0]);
		}

		vertexBuffers.Insert(Engine::GetRenderer()->GetAllocator()->CreateVertexBuffer(vertices, Vertex3D::GetVertexInfo()).GetPointer());
		indexBuffers.Insert(Engine::GetRenderer()->GetAllocator()->CreateIndexBuffer(indices).GetPointer());
	}

	if (!bottomLevelVertexBuffers.ContainsKey(obj)) {
		bottomLevelVertexBuffers.Insert(obj, {});
		bottomLevelIndexBuffers.Insert(obj, {});
	}

	for (const auto& v : vertexBuffers)
		bottomLevelVertexBuffers.Get(obj).Insert(v.GetPointer());

	for (const auto& i : indexBuffers)
		bottomLevelIndexBuffers.Get(obj).Insert(i.GetPointer());
}
