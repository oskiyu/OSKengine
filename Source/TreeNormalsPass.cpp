#include "TreeNormalsPass.h"

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


void TreeNormalsPass::Load() {
	m_passMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/Deferred/Tree/tree_normals.json");
	m_materialInstance = m_passMaterial->CreateInstance().GetPointer();
}

void TreeNormalsPass::RenderLoop(ICommandList* commandList, const DynamicArray<ECS::GameObjectIndex>& objectsToRender, UIndex32 jitterIndex, Vector2ui resolution) {
	commandList->StartDebugSection("Tree Normals Pass", Color::Red);

	const auto frustum =
		Engine::GetEcs()->GetComponent<CameraComponent3D>(m_cameraObject)
		.GetFrustum(Engine::GetEcs()->GetComponent<Transform3D>(m_cameraObject));

	PushConstants modelPushConstants{};

	commandList->BindMaterial(*m_passMaterial);
	commandList->BindMaterialSlot(*m_materialInstance->GetSlot("global"));

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

			modelPushConstants.modelMatrix = transform.GetAsMatrix();
			modelPushConstants.resolution = resolution.ToVector2f();
			modelPushConstants.jitterIndex = static_cast<float>(jitterIndex);

			commandList->PushMaterialConstants("model", modelPushConstants);

			commandList->DrawSingleMesh(mesh.GetFirstIndexId(), mesh.GetNumberOfIndices());
		}
	}

	commandList->EndDebugSection();
}
