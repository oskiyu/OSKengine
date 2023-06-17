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
	pointMaterial = Engine::GetRenderer()->GetMaterialSystem()
		->LoadMaterial("Resources/Materials/Collision/collision_point_material.json");

	materialInstance = material->CreateInstance().GetPointer();
	pointsMaterialInstance = pointMaterial->CreateInstance().GetPointer();

	// Asset load
	cubeModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/Colliders/cube.json", "ColliderRenderSystem");
	sphereModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/Colliders/sphere.json", "ColliderRenderSystem");

	// Material setup
	const GpuBuffer* _cameraUbos[NUM_RESOURCES_IN_FLIGHT]{};
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		cameraUbos[i] = Engine::GetRenderer()->GetAllocator()
			->CreateUniformBuffer(sizeof(glm::mat4) * 2 + sizeof(glm::vec4))
			.GetPointer();
		_cameraUbos[i] = cameraUbos[i].GetPointer();
	}

	materialInstance->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
	materialInstance->GetSlot("global")->FlushUpdate();

	pointsMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
	pointsMaterialInstance->GetSlot("global")->FlushUpdate();
}

void ColliderRenderSystem::Initialize(GameObjectIndex camera) {
	this->cameraObject = camera;
}

void ColliderRenderSystem::CreateTargetImage(const Vector2ui& size) {
	RenderTargetAttachmentInfo colorInfo{ .format = Format::RGBA8_SRGB, .name = "Collider Render System Target" };
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D16_UNORM, .name = "Collider Render System Depth" };
	renderTarget.Create(size, { colorInfo }, depthInfo);
}

void ColliderRenderSystem::Render(GRAPHICS::ICommandList* commandList) {
	if (cameraObject == EMPTY_GAME_OBJECT)
		return;

	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	const CameraComponent3D& camera = Engine::GetEcs()->GetComponent<CameraComponent3D>(cameraObject);
	const Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(cameraObject);

	cameraUbos[resourceIndex]->MapMemory();
	cameraUbos[resourceIndex]->Write(camera.GetProjectionMatrix());
	cameraUbos[resourceIndex]->Write(camera.GetViewMatrix(cameraTransform));
	cameraUbos[resourceIndex]->Write(cameraTransform.GetPosition());
	cameraUbos[resourceIndex]->Unmap();

	// Información por cada entidad.
	struct RenderInfo {
		glm::mat4 modelMatrix;
		Color color;
	} renderInfo {};

	commandList->StartDebugSection("Collision Render", Color::Red);

	commandList->BeginGraphicsRenderpass(&renderTarget, Color::Black * 0.0f);
	SetupViewport(commandList);


	// Obtenemos todas las entidades que están colisionando,
	// para poder cambiarles de color.
	const auto& eventQueue = Engine::GetEcs()->GetEventQueue<CollisionEvent>();
	DynamicArray<Vector3f> contactPoints;
	DynamicArray<Vector3f> singleContactPoints;
	std::set<GameObjectIndex> collisionObjects;
	for (const CollisionEvent& ev : eventQueue) {
		collisionObjects.insert(ev.firstEntity);
		collisionObjects.insert(ev.secondEntity);
		contactPoints.InsertAll(ev.collisionInfo.GetContactPoints());
		singleContactPoints.Insert(ev.collisionInfo.GetSingleContactPoint());
	}

	for (GameObjectIndex gameObject : GetObjects()) {
		commandList->BindMaterial(*material);
		commandList->BindMaterialSlot(*materialInstance->GetSlot("global"));

		const Transform3D& originalTransform = Engine::GetEcs()->GetComponent<Transform3D>(gameObject);
		Transform3D topLevelTransform = originalTransform;
		topLevelTransform.SetRotation({});

		const Collider& collider = Engine::GetEcs()->GetComponent<Collider>(gameObject);
		const ITopLevelCollider* topLevelCollider = collider.GetTopLevelCollider();

		if (collisionObjects.contains(gameObject))
			renderInfo.color = Color::Red * 0.75f;
		else
			renderInfo.color = Color::Yellow * 0.75f;

		if (auto* box = dynamic_cast<const AxisAlignedBoundingBox*>(topLevelCollider)) {
			topLevelTransform.SetScale(box->GetSize());
			renderInfo.modelMatrix = topLevelTransform.GetAsMatrix();
			commandList->PushMaterialConstants("pushConstants", renderInfo);

			commandList->BindVertexBuffer(*cubeModel->GetVertexBuffer());
			commandList->BindIndexBuffer(*cubeModel->GetIndexBuffer());
			commandList->DrawSingleInstance(cubeModel->GetIndexCount());
		} else
		if (auto* sphere = dynamic_cast<const SphereCollider*>(topLevelCollider)) {
			topLevelTransform.SetScale(sphere->GetRadius());
			renderInfo.modelMatrix = topLevelTransform.GetAsMatrix();
			commandList->PushMaterialConstants("pushConstants", renderInfo);

			commandList->BindVertexBuffer(*sphereModel->GetVertexBuffer());
			commandList->BindIndexBuffer(*sphereModel->GetIndexBuffer());
			commandList->DrawSingleInstance(sphereModel->GetIndexCount());
		}

		if (bottomLevelVertexBuffers.ContainsKey(gameObject)) {
			const auto& vertexBuffers = bottomLevelVertexBuffers.Get(gameObject);
			const auto& indexBuffers = bottomLevelIndexBuffers.Get(gameObject);

			if (collisionObjects.contains(gameObject))
				renderInfo.color = Color::Red;
			else
				renderInfo.color = Color::Yellow;

			renderInfo.modelMatrix = originalTransform.GetAsMatrix();
			commandList->PushMaterialConstants("pushConstants", renderInfo);

			commandList->BindMaterial(*lowLevelMaterial);
			for (UIndex64 i = 0; i < vertexBuffers.GetSize(); i++) {
				commandList->BindVertexBuffer(*vertexBuffers[i].GetPointer());
				commandList->BindIndexBuffer(*indexBuffers[i].GetPointer());
				commandList->DrawSingleInstance(indexBuffers[i]->GetIndexView().numIndices);
			}
		}
	}

	commandList->BindVertexBuffer(*cubeModel->GetVertexBuffer());
	commandList->BindIndexBuffer(*cubeModel->GetIndexBuffer());

	commandList->BindMaterial(*pointMaterial);
	commandList->BindMaterialSlot(*pointsMaterialInstance->GetSlot("global"));

	struct {
		Vector4f point;
		Color color = Color::Red;
	} pushConstant;

	for (const Vector3f point : contactPoints) {
		pushConstant.point = Vector4f(point.x, point.y, point.Z, 1.0f);
		commandList->PushMaterialConstants("pushConstants", pushConstant);
		commandList->DrawSingleInstance(1);
	}

	pushConstant.color = Color::Purple;
	for (const Vector3f point : singleContactPoints) {
		pushConstant.point = Vector4f(point.x, point.y, point.Z, 1.0f);
		commandList->PushMaterialConstants("pushConstants", pushConstant);
		commandList->DrawSingleInstance(1);
	}

	commandList->EndGraphicsRenderpass();

	commandList->EndDebugSection();
}

void ColliderRenderSystem::OnObjectAdded(GameObjectIndex obj) {
	SetupBottomLevelModel(obj);
}

void ColliderRenderSystem::OnObjectRemoved(GameObjectIndex obj) {
	if (bottomLevelVertexBuffers.ContainsKey(obj)) {
		bottomLevelVertexBuffers.Get(obj).Empty();
		bottomLevelIndexBuffers.Get(obj).Empty();
	}
}

void ColliderRenderSystem::SetupBottomLevelModel(GameObjectIndex obj) {
	const Collider& collider = Engine::GetEcs()->GetComponent<Collider>(obj);
	
	if (bottomLevelVertexBuffers.ContainsKey(obj)) {
		bottomLevelVertexBuffers.Get(obj).Empty();
		bottomLevelIndexBuffers.Get(obj).Empty();
	}

	// Listas con los vertex e index buffers de la entidad.
	// Cada ConvexVolume tendrá uno.
	DynamicArray<OwnedPtr<GpuBuffer>> vertexBuffers;
	DynamicArray<OwnedPtr<GpuBuffer>> indexBuffers;

	
	for (UIndex64 c = 0; c < collider.GetBottomLevelCollidersCount(); c++) {
		const auto& blc = *collider.GetBottomLevelCollider(c)->As<ConvexVolume>();

		// "Convertimos" los vértices del collider
		// en vértices de renderizado.
		DynamicArray<Vertex3D> vertices;
		for (const auto& cVertex : blc.GetLocalSpaceVertices()){
			Vertex3D vertex{};

			vertex.position = cVertex;
			vertex.normal = 0.0f;
			vertex.color = Color::White;
			vertex.texCoords = Vector2f::Zero;

			vertices.Insert(vertex);
		}

		// Índices de vértices.
		DynamicArray<TIndexSize> indices;
		for (const auto& face : blc.GetFaceIndices()) {
			for (const auto index : face)
				indices.Insert(index);

			// Re-añadimos el último vértice de la geometría,
			// para "cerrar" el polígono, creando una
			// línea entre el primer y el último vértice.
			indices.Insert(face[0]);
		}

		// Creamos los buffers.
		vertexBuffers.Insert(Engine::GetRenderer()->GetAllocator()->CreateVertexBuffer(vertices, Vertex3D::GetVertexInfo()).GetPointer());
		indexBuffers.Insert(Engine::GetRenderer()->GetAllocator()->CreateIndexBuffer(indices).GetPointer());
	}

	// Si no existían las listas, las introducimos primero.
	if (!bottomLevelVertexBuffers.ContainsKey(obj)) {
		bottomLevelVertexBuffers.Insert(obj, {});
		bottomLevelIndexBuffers.Insert(obj, {});
	}

	for (const auto& v : vertexBuffers)
		bottomLevelVertexBuffers.Get(obj).Insert(v.GetPointer());

	for (const auto& i : indexBuffers)
		bottomLevelIndexBuffers.Get(obj).Insert(i.GetPointer());
}
