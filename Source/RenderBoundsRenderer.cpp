#include "RenderBoundsRenderer.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "Model3D.h"
#include "ModelComponent3D.h"
#include "CameraComponent3D.h"
#include "Transform3D.h"
#include "MatrixOperations.hpp"

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
	m_materialInstance = m_material->CreateInstance().GetPointer();

	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> cameraUbos{};
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		m_cameraUbos[i] = Engine::GetRenderer()->GetAllocator()
			->CreateUniformBuffer(sizeof(glm::mat4) * 2 + sizeof(glm::vec4))
			.GetPointer();

		cameraUbos[i] = m_cameraUbos[i].GetPointer();
	}

	m_materialInstance->GetSlot("global")->SetUniformBuffers("camera", cameraUbos);
	m_materialInstance->GetSlot("global")->FlushUpdate();
}

void RenderBoundsRenderer::Initialize(GameObjectIndex camera) {
	m_cameraObject = camera;
}

void RenderBoundsRenderer::CreateTargetImage(const Vector2ui& size) {
	RenderTargetAttachmentInfo colorInfo{ .format = Format::RGBA8_SRGB, .name = "Bounds Render System Target" };
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D16_UNORM, .name = "Bounds Render System Depth" };
	m_renderTarget.Create(size, { colorInfo }, depthInfo);
}

void RenderBoundsRenderer::Render(ICommandList* commandList) {
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
	commandList->BindMaterialInstance(*m_materialInstance);

	commandList->BindVertexBuffer(*m_sphereModel->GetVertexBuffer());
	commandList->BindIndexBuffer(*m_sphereModel->GetIndexBuffer());

	Transform3D transform(EMPTY_GAME_OBJECT);

	struct RenderInfo {
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		Color color = Color::Yellow;
	};
	RenderInfo info{};

	for (const GameObjectIndex obj : GetObjects()) {
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
			commandList->DrawSingleInstance(m_sphereModel->GetIndexCount());
		}

	}

	commandList->EndGraphicsRenderpass();

	commandList->EndDebugSection();
}
