#include "TreeGBufferPass.h"

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


void TreeGBufferPass::Load() {
	m_passMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/Deferred/Tree/tree_gbuffer.json");
	m_materialInstance = m_passMaterial->CreateInstance().GetPointer();
}

void TreeGBufferPass::RenderLoop(ICommandList* commandList, const DynamicArray<ECS::GameObjectIndex>& objectsToRender, UIndex32 jitterIndex, Vector2ui resolution) {
	commandList->StartDebugSection("Tree Pass", Color::Red);

	const auto frustum =
		Engine::GetEcs()->GetComponent<CameraComponent3D>(m_cameraObject)
		.GetFrustum(Engine::GetEcs()->GetComponent<Transform3D>(m_cameraObject));

	DeferredPushConstants modelPushConstants{};

	commandList->BindMaterial(*m_passMaterial);
	commandList->BindMaterialSlot(*m_materialInstance->GetSlot("normals"));

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
			auto& mesh = model.GetModel()->GetMeshes()[i];
			const bool isInsideFrustum = mesh.GetBounds().IsInsideFrustum(frustum);

			if (!isInsideFrustum)
				continue;

			if (!m_meshMapping.HasModel(model.GetModel()->GetId()))
				m_meshMapping.RegisterModel(model.GetModel()->GetId());

			auto& modelData = m_meshMapping.GetModelData(model.GetModel()->GetId());
			if (!modelData.HasMesh(mesh.GetMeshId()))
				SetupMaterialInstance(*model.GetModel(), mesh);

			const auto& mSlot = *modelData.GetMeshData(mesh.GetMeshId()).GetMaterialInstance()->GetSlot("texture");
			commandList->BindMaterialSlot(mSlot);

			modelPushConstants.model = transform.GetAsMatrix();
			modelPushConstants.previousModel = m_previousModelMatrices.contains(obj) ? m_previousModelMatrices.at(obj) : glm::mat4(1.0f);
			modelPushConstants.resolution = resolution.ToVector2f();
			modelPushConstants.jitterIndex = (float)jitterIndex;

			commandList->PushMaterialConstants("model", modelPushConstants);

			commandList->DrawSingleMesh(mesh.GetFirstIndexId(), mesh.GetNumberOfIndices());
		}

		m_previousModelMatrices[obj] = transform.GetAsMatrix();
	}

	commandList->EndDebugSection();
}
