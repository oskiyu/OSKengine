// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#include "ColliderRenderer.h"

#include "OSKengine.h"
#include "AssetManager.h"
#include "Model3D.h"
#include "Material.h"

#include "Transform3D.h"
#include "CollisionComponent.h"
#include "CameraComponent3D.h"

#include "AxisAlignedBoundingBox.h"
#include "SphereCollider.h"
#include "ConvexVolume.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "CollisionEvent.h"
#include "Vertex3D.h"

#include "IGpuMemoryAllocator.h"
#include "CollisionSystem.h"

#include "SavedGameObjectTranslator.h"

#include <set>
#include "GpuQueueTypes.h"
#include "GpuBuffer.h"
#include "ResourcesInFlight.h"
#include "NumericTypes.h"
#include "Uuid.h"
#include "GameObject.h"
#include "Format.h"
#include "Vector2.hpp"
#include "RenderTargetAttachmentInfo.h"
#include "ICommandList.h"
#include <span>

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;
using namespace OSK::COLLISION;

ColliderRenderSystem::ColliderRenderSystem() {
	Signature signature{};
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<CollisionComponent>());
	_SetSignature(signature);

	// Material load
	{
		material = Engine::GetRenderer()->GetMaterialSystem()
			->LoadMaterial("Resources/Materials/Collision/collision_material.json");
		lowLevelMaterial = Engine::GetRenderer()->GetMaterialSystem()
			->LoadMaterial("Resources/Materials/Collision/lowlevel_collision_material.json");
		pointMaterial = Engine::GetRenderer()->GetMaterialSystem()
			->LoadMaterial("Resources/Materials/Collision/collision_point_material.json");

		for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
			m_materialInstances[i]		 = material->CreateInstance().GetPointer();
			m_pointsMaterialInstances[i] = pointMaterial->CreateInstance().GetPointer();
		}
	}

	// Asset load
	{
		m_cubeModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/Colliders/cube.json");
		m_sphereModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/Colliders/sphere.json");
	}

	// Material setup
	{
		constexpr USize64 cameraBufferSize = sizeof(glm::mat4) * 2 + sizeof(glm::vec4);

		for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
			IGpuMemoryAllocator* allocator = Engine::GetRenderer()->GetAllocator();
			OwnedPtr<GpuBuffer> buffer = allocator->CreateUniformBuffer(cameraBufferSize, GpuQueueType::MAIN);

			m_materialInstances[i]->GetSlot("global")->SetUniformBuffer("camera", buffer.GetValue());
			m_materialInstances[i]->GetSlot("global")->FlushUpdate();

			m_pointsMaterialInstances[i]->GetSlot("global")->SetUniformBuffer("camera", buffer.GetValue());
			m_pointsMaterialInstances[i]->GetSlot("global")->FlushUpdate();

			m_cameraGpuBuffers[i] = buffer.GetPointer();
		}
	}
}

void ColliderRenderSystem::Initialize(GameObjectIndex camera) {
	m_cameraObject = camera;
}

void ColliderRenderSystem::CreateTargetImage(const Vector2ui& size) {
	RenderTargetAttachmentInfo colorInfo{ .format = Format::RGBA8_SRGB, .name = "Collider Render System Target" };
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D16_UNORM, .name = "Collider Render System Depth" };
	m_renderTarget.Create(size, { colorInfo }, depthInfo);
}

void ColliderRenderSystem::Render(GRAPHICS::ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) {
	if (m_cameraObject == EMPTY_GAME_OBJECT) {
		return;
	}

	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	const CameraComponent3D& camera = Engine::GetEcs()->GetComponent<CameraComponent3D>(m_cameraObject);
	const Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(m_cameraObject);

	m_cameraGpuBuffers[resourceIndex]->MapMemory();
	m_cameraGpuBuffers[resourceIndex]->Write(camera.GetProjectionMatrix());
	m_cameraGpuBuffers[resourceIndex]->Write(camera.GetViewMatrix(cameraTransform));
	m_cameraGpuBuffers[resourceIndex]->Write(cameraTransform.GetPosition());
	m_cameraGpuBuffers[resourceIndex]->Unmap();

	// Información por cada entidad.
	struct RenderInfo {
		glm::mat4 modelMatrix;
		Color color;
	} renderInfo {};

	commandList->StartDebugSection("Collision Render", Color::Red);

	commandList->BeginGraphicsRenderpass(&m_renderTarget, Color::Black * 0.0f);
	SetupViewport(commandList);


	// Obtenemos todas las entidades que están colisionando,
	// para poder cambiarles de color.
	const auto& eventQueue = Engine::GetEcs()->GetEventQueue<CollisionEvent>();
	DynamicArray<Vector3f> contactPoints;
	DynamicArray<Vector3f> singleContactPoints;
	DynamicArray<Vector3f> rayCastContactPoints;
	std::set<GameObjectIndex> collisionObjects;
	std::set<GameObjectIndex> rayCastedObjects;

	for (const CollisionEvent& ev : eventQueue) {
		collisionObjects.insert(ev.firstEntity);
		collisionObjects.insert(ev.secondEntity);
		contactPoints.InsertAll(ev.collisionInfo.GetContactPoints());
		singleContactPoints.Insert(ev.collisionInfo.GetSingleContactPoint());
	}

	for (GameObjectIndex gameObject : objects) {
		commandList->BindMaterial(*material);
		commandList->BindMaterialSlot(*m_materialInstances[resourceIndex]->GetSlot("global"));

		const Transform3D& originalTransform = Engine::GetEcs()->GetComponent<Transform3D>(gameObject);
		Transform3D topLevelTransform = originalTransform;
		topLevelTransform.SetRotation({});

		singleContactPoints.Insert(originalTransform.GetPosition());

		if (gameObject.Get() == 1) {
			COLLISION::Ray ray{};
			ray.origin = originalTransform.GetPosition() + Vector3f(0.0f, 0.11f, 0.0f);
			ray.direction = originalTransform.GetForwardVector();

			const auto rayResult = Engine::GetEcs()->GetSystem<CollisionSystem>()->CastRay(ray, gameObject);
			if (rayResult.Result()) {
				rayCastContactPoints.Insert(rayResult.GetIntersectionPoint());
				rayCastedObjects.insert(rayResult.GetObject());
			}
		}

		const Collider& collider = *Engine::GetEcs()->GetComponent<CollisionComponent>(gameObject).GetCollider();
		const ITopLevelCollider* topLevelCollider = collider.GetTopLevelCollider();

		if (rayCastedObjects.contains(gameObject))
			renderInfo.color = Color::Green * 0.75f;
		// else if (collisionObjects.contains(gameObject))
		//	renderInfo.color = Color::Red * 0.75f;
		else
			renderInfo.color = Color::Yellow * 0.75f;

		if (auto* box = dynamic_cast<const AxisAlignedBoundingBox*>(topLevelCollider)) {
			topLevelTransform.SetScale(box->GetSize());
			renderInfo.modelMatrix = topLevelTransform.GetAsMatrix();
			commandList->PushMaterialConstants("pushConstants", renderInfo);

			commandList->BindVertexBuffer(m_cubeModel.GetAsset()->GetModel().GetVertexBuffer());
			commandList->BindIndexBuffer(m_cubeModel.GetAsset()->GetModel().GetIndexBuffer());
			commandList->DrawSingleInstance(m_cubeModel.GetAsset()->GetModel().GetTotalIndexCount());
		} else
		if (auto* sphere = dynamic_cast<const SphereCollider*>(topLevelCollider)) {
			topLevelTransform.SetScale(Vector3f(sphere->GetRadius()));
			renderInfo.modelMatrix = topLevelTransform.GetAsMatrix();
			commandList->PushMaterialConstants("pushConstants", renderInfo);

			commandList->BindVertexBuffer(m_sphereModel.GetAsset()->GetModel().GetVertexBuffer());
			commandList->BindIndexBuffer(m_sphereModel.GetAsset()->GetModel().GetIndexBuffer());
			commandList->DrawSingleInstance(m_sphereModel.GetAsset()->GetModel().GetTotalIndexCount());
		}

		if (m_bottomLevelVertexBuffers.contains(gameObject)) {
			const auto& vertexBuffers = m_bottomLevelVertexBuffers.at(gameObject);
			const auto& indexBuffers = m_bottomLevelIndexBuffers.at(gameObject);

			if (rayCastedObjects.contains(gameObject))
				renderInfo.color = Color::Green * 0.75f;
			// else if (collisionObjects.contains(gameObject))
			//	renderInfo.color = Color::Red * 0.75f;
			else
				renderInfo.color = Color::Yellow * 0.75f;

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

	commandList->BindVertexBuffer(m_cubeModel.GetAsset()->GetModel().GetVertexBuffer());
	commandList->BindIndexBuffer(m_cubeModel.GetAsset()->GetModel().GetIndexBuffer());

	commandList->BindMaterial(*pointMaterial);
	commandList->BindMaterialSlot(*m_pointsMaterialInstances[resourceIndex]->GetSlot("global"));

	struct {
		Vector4f point;
		Color color = Color::Red;
	} pushConstant;

	for (const Vector3f point : contactPoints) {
		pushConstant.point = Vector4f(point.x, point.y, point.z, 1.0f);
		commandList->PushMaterialConstants("pushConstants", pushConstant);
		commandList->DrawSingleInstance(1);
	}

	pushConstant.color = Color::Purple;
	for (const Vector3f point : singleContactPoints) {
		pushConstant.point = Vector4f(point.x, point.y, point.z, 1.0f);
		commandList->PushMaterialConstants("pushConstants", pushConstant);
		commandList->DrawSingleInstance(1);
	}

	pushConstant.color = Color::Green;
	for (const Vector3f point : rayCastContactPoints) {
		pushConstant.point = Vector4f(point.x, point.y, point.z, 1.0f);
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
	if (m_bottomLevelVertexBuffers.contains(obj)) {
		m_bottomLevelVertexBuffers.at(obj).Empty();
		m_bottomLevelIndexBuffers.at(obj).Empty();
	}
}

void ColliderRenderSystem::SetupBottomLevelModel(GameObjectIndex obj) {
	const Collider& collider = *Engine::GetEcs()->GetComponent<CollisionComponent>(obj).GetCollider();
	
	if (m_bottomLevelVertexBuffers.contains(obj)) {
		m_bottomLevelVertexBuffers.at(obj).Empty();
		m_bottomLevelIndexBuffers.at(obj).Empty();
	}

	// Listas con los vertex e index buffers de la entidad.
	// Cada ConvexVolume tendrá uno.
	DynamicArray<OwnedPtr<GpuBuffer>> vertexBuffers;
	DynamicArray<OwnedPtr<GpuBuffer>> indexBuffers;

	
	for (UIndex32 c = 0; c < collider.GetBottomLevelCollidersCount(); c++) {
		const auto& blc = *collider.GetBottomLevelCollider(c)->As<ConvexVolume>();

		// "Convertimos" los vértices del collider
		// en vértices de renderizado.
		DynamicArray<Vertex3D> vertices;
		for (const auto& cVertex : blc.GetLocalSpaceVertices()){
			Vertex3D vertex{};

			vertex.position = cVertex;
			vertex.normal = Vector3f::Zero;
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
		vertexBuffers.Insert(Engine::GetRenderer()->GetAllocator()->CreateVertexBuffer(
			vertices, 
			Vertex3D::GetVertexInfo(),
			GpuQueueType::MAIN).GetPointer());

		indexBuffers.Insert(Engine::GetRenderer()->GetAllocator()->CreateIndexBuffer(indices, GpuQueueType::MAIN).GetPointer());
	}

	// Si no existían las listas, las introducimos primero.
	if (!m_bottomLevelVertexBuffers.contains(obj)) {
		m_bottomLevelVertexBuffers[obj] = {};
		m_bottomLevelIndexBuffers[obj] = {};
	}

	for (const auto& v : vertexBuffers) {
		m_bottomLevelVertexBuffers.at(obj).Insert(v.GetPointer());
	}

	for (const auto& i : indexBuffers) {
		m_bottomLevelIndexBuffers.at(obj).Insert(i.GetPointer());
	}
}


const GRAPHICS::Material* ColliderRenderSystem::GetTopLevelMaterial() const {
	return material;
}

const GRAPHICS::Material* ColliderRenderSystem::GetBottomLevelMaterial() const {
	return lowLevelMaterial;
}

const GRAPHICS::Material* ColliderRenderSystem::GetPointsMaterial() const {
	return pointMaterial;
}

GameObjectIndex ColliderRenderSystem::GetCurrentCameraObject() const {
	return m_cameraObject;
}


nlohmann::json ColliderRenderSystem::SaveConfiguration() const {
	auto output = nlohmann::json();

	output["cameraObject"] = m_cameraObject.Get();

	// cubeModel y sphereModel se cargan al crear el sistema:
	// no es necesario serializarlo.

	return output;
}

PERSISTENCE::BinaryBlock ColliderRenderSystem::SaveBinaryConfiguration() const {
	auto data =  PERSISTENCE::BinaryBlock::Empty();
	
	data.Write(m_cameraObject.Get());
	
	return data;
}

void ColliderRenderSystem::ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) {
	m_cameraObject = translator.GetCurrentIndex(GameObjectIndex(config["cameraObject"]));
	UpdatePassesCamera(m_cameraObject);
	// cubeModel y sphereModel se cargan al crear el sistema:
	// no es necesario cargarlo de nuevo.
}

void ColliderRenderSystem::ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) {
	m_cameraObject = translator.GetCurrentIndex(GameObjectIndex(reader->Read<GameObjectIndex::TUnderlyingType>()));
	UpdatePassesCamera(m_cameraObject);
	// cubeModel y sphereModel se cargan al crear el sistema:
	// no es necesario cargarlo de nuevo.
}
