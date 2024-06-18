#include "RenderBoundsRenderer.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "Model3D.h"
#include "ModelComponent3D.h"
#include "CameraComponent3D.h"
#include "Transform3D.h"
#include "MatrixOperations.hpp"

#include "SavedGameObjectTranslator.h"


using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

RenderBoundsRenderer::RenderBoundsRenderer() {
	Signature signature{};
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<ModelComponent3D>());
	_SetSignature(signature);

	m_sphereModel = Engine::GetAssetManager()->Load<Model3D>("Resources/Assets/Models/Colliders/sphere_low.json");

	LoadMaterials();
}

void RenderBoundsRenderer::LoadMaterials() {
	m_material = Engine::GetRenderer()->GetMaterialSystem()
		->LoadMaterial("Resources/Materials/Collision/collision_material.json");

	for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		m_cameraUbos[i] = Engine::GetRenderer()->GetAllocator()
			->CreateUniformBuffer(sizeof(glm::mat4) * 2 + sizeof(glm::vec4), GpuQueueType::MAIN)
			.GetPointer();

		m_materialInstance[i] = m_material->CreateInstance().GetPointer();
		m_materialInstance[i]->GetSlot("global")->SetUniformBuffer("camera", m_cameraUbos[i].GetValue());
		m_materialInstance[i]->GetSlot("global")->FlushUpdate();
	}
}

void RenderBoundsRenderer::Initialize(GameObjectIndex camera) {
	m_cameraObject = camera;
}

void RenderBoundsRenderer::CreateTargetImage(const Vector2ui& size) {
	RenderTargetAttachmentInfo colorInfo{ .format = Format::RGBA8_SRGB, .name = "Bounds Render System Target" };
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D16_UNORM, .name = "Bounds Render System Depth" };
	m_renderTarget.Create(size, { colorInfo }, depthInfo);
}

void RenderBoundsRenderer::Render(ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) {
	if (m_cameraObject == EMPTY_GAME_OBJECT)
		return;

	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	const CameraComponent3D& camera = Engine::GetEcs()->GetComponent<CameraComponent3D>(m_cameraObject);
	const Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(m_cameraObject);

	m_cameraUbos[resourceIndex]->MapMemory();
	m_cameraUbos[resourceIndex]->Write(camera.GetProjectionMatrix());
	m_cameraUbos[resourceIndex]->Write(camera.GetViewMatrix(cameraTransform));
	m_cameraUbos[resourceIndex]->Write(cameraTransform.GetPosition());
	m_cameraUbos[resourceIndex]->Unmap();

	// Render
	commandList->StartDebugSection("Bounds Rendering", Color::Red);

	commandList->BeginGraphicsRenderpass(&m_renderTarget, Color::Black * 0.0f);
	SetupViewport(commandList);

	commandList->BindMaterial(*m_material);
	commandList->BindMaterialInstance(*m_materialInstance[Engine::GetRenderer()->GetCurrentResourceIndex()]);

	commandList->BindVertexBuffer(m_sphereModel->GetModel().GetVertexBuffer());
	commandList->BindIndexBuffer(m_sphereModel->GetModel().GetIndexBuffer());

	Transform3D transform(EMPTY_GAME_OBJECT);

	struct RenderInfo {
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		Color color = Color::Yellow;
	};
	RenderInfo info{};

	for (const GameObjectIndex obj : objects) {
		const auto& model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);
		const auto& objectTransform = Engine::GetEcs()->GetComponent<Transform3D>(obj);

		for (const auto& mesh : model.GetModel()->GetMeshes()) {
			const auto& sphere = mesh.GetBounds();

			const Vector3f spherePosition = Math::TransformPoint(mesh.GetSphereCenter(), objectTransform.GetAsMatrix());

			transform.SetPosition(spherePosition);
			transform.SetScale(Vector3f(sphere.GetRadius()));

			const float distance = cameraTransform.GetPosition().GetDistanceTo(spherePosition) - sphere.GetRadius();

			if (distance > 20.0f)
				continue;

			info.modelMatrix = transform.GetAsMatrix();
			info.color.alpha = 20.0f / distance;

			commandList->PushMaterialConstants("pushConstants", info);
			commandList->DrawSingleInstance(m_sphereModel->GetModel().GetTotalIndexCount());
		}

	}

	commandList->EndGraphicsRenderpass();

	commandList->EndDebugSection();
}

nlohmann::json RenderBoundsRenderer::SaveConfiguration() const {
	auto output = nlohmann::json();

	output["cameraObject"] = m_cameraObject.Get();

	// m_sphereModel se cargan al crear el sistema:
	// no es necesario serializarlo.

	return output;
}

PERSISTENCE::BinaryBlock RenderBoundsRenderer::SaveBinaryConfiguration() const {
	auto output = PERSISTENCE::BinaryBlock::Empty();

	output.Write(m_cameraObject.Get());

	return output;
}

void RenderBoundsRenderer::ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) {
	m_cameraObject = translator.GetCurrentIndex(GameObjectIndex(config["cameraObject"]));
	UpdatePassesCamera(m_cameraObject);
	// m_sphereModel se cargan al crear el sistema:
	// no es necesario cargarlo de nuevo.
}

void RenderBoundsRenderer::ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) {
	m_cameraObject = translator.GetCurrentIndex(GameObjectIndex(reader->Read<GameObjectIndex::TUnderlyingType>()));
	UpdatePassesCamera(m_cameraObject);
	// cubeModel y sphereModel se cargan al crear el sistema:
	// no es necesario cargarlo de nuevo.
}
