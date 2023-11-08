#include "BillboardGBufferPass.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"

#include "CameraComponent3D.h"
#include "Transform3D.h"
#include "ModelComponent3D.h"
#include "Model3D.h"
#include "DeferredPushConstants.h"

#include "MatrixOperations.hpp"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


void BillboardGBufferPass::Load() {
	m_passMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/Deferred/deferred_gbuffer_billboard.json");
}

void BillboardGBufferPass::RenderLoop(ICommandList* commandList, const DynamicArray<ECS::GameObjectIndex>& objectsToRender, UIndex32 jitterIndex, Vector2ui resolution) {
	commandList->StartDebugSection("Billboards Pass", Color::Red);

	const auto frustum =
		Engine::GetEcs()->GetComponent<CameraComponent3D>(m_cameraObject)
		.GetFrustum(Engine::GetEcs()->GetComponent<Transform3D>(m_cameraObject));

	DeferredPushConstants modelPushConstants{};

	commandList->BindMaterial(*m_passMaterial);

	// "Caché" de buffers.
	const GpuBuffer* previousVertexBuffer = nullptr;
	const GpuBuffer* previousIndexBuffer = nullptr;

	for (GameObjectIndex obj : objectsToRender) {
		const ModelComponent3D& model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);
		const Transform3D& transform = Engine::GetEcs()->GetComponent<Transform3D>(obj);

		// Actualizamos el modelo 3D, si es necesario.
		if (previousVertexBuffer != model.GetModel()->GetVertexBuffer()) {
			commandList->BindVertexBuffer(*model.GetModel()->GetVertexBuffer());
			previousVertexBuffer = model.GetModel()->GetVertexBuffer();
		}

		if (previousIndexBuffer != model.GetModel()->GetIndexBuffer()) {
			commandList->BindIndexBuffer(*model.GetModel()->GetIndexBuffer());
			previousIndexBuffer = model.GetModel()->GetIndexBuffer();
		}

		for (UIndex32 i = 0; i < model.GetModel()->GetMeshes().GetSize(); i++) {
			const auto& mesh = model.GetModel()->GetMeshes()[i];
			const auto& sphere = mesh.GetBounds();
			const Vector3f spherePosition = Math::TransformPoint(mesh.GetSphereCenter(), transform.GetAsMatrix());

			const bool isInsideFrustum = sphere.IsInsideFrustum(frustum, spherePosition);

			if (!isInsideFrustum)
				continue;

			if (!m_meshMapping.HasModel(model.GetModel()->GetId()))
				m_meshMapping.RegisterModel(model.GetModel()->GetId());

			auto& modelData = m_meshMapping.GetModelData(model.GetModel()->GetId());
			if (!modelData.HasMesh(mesh.GetMeshId()))
				SetupMaterialInstance(*model.GetModel(), mesh);

			const auto& mSlot = *modelData.GetMeshData(mesh.GetMeshId()).GetMaterialInstance()->GetSlot("texture");
			commandList->BindMaterialSlot(mSlot);

			const Vector4f materialInfo{
				model.GetModel()->GetMetadata().meshesMetadata[i].metallicFactor,
				model.GetModel()->GetMetadata().meshesMetadata[i].roughnessFactor,
				(float)jitterIndex,
				0.0f
			};

			modelPushConstants.model = transform.GetAsMatrix();
			modelPushConstants.previousModel = m_previousModelMatrices.contains(obj) ? m_previousModelMatrices.at(obj) : glm::mat4(1.0f);
			modelPushConstants.materialInfo = materialInfo;
			modelPushConstants.resolution = resolution.ToVector2f();

			commandList->PushMaterialConstants("model", modelPushConstants);

			commandList->DrawSingleMesh(mesh.GetFirstIndexId(), mesh.GetNumberOfIndices());
		}

		m_previousModelMatrices[obj] = transform.GetAsMatrix();
	}

	commandList->EndDebugSection();
}
