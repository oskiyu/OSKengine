#include "StaticGBufferPass.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"

#include "CameraComponent3D.h"
#include "TransformComponent3D.h"
#include "ModelComponent3D.h"
#include "Model3D.h"
#include "DeferredPushConstants.h"
#include "Texture.h"

#include "MatrixOperations.hpp"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


void StaticGBufferPass::Load() {
	m_passMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/Deferred/deferred_gbuffer_static.json");
}

void StaticGBufferPass::RenderLoop(ICommandList* commandList, const DynamicArray<ECS::GameObjectIndex>& objectsToRender, GlobalMeshMapping* globalMeshMapping, UIndex32 jitterIndex, Vector2ui resolution) {
	commandList->StartDebugSection("Static Meshes Pass", Color::Red);
	
	const auto frustum =
		Engine::GetEcs()->GetComponent<CameraComponent3D>(m_cameraObject)
		.GetFrustum(Engine::GetEcs()->GetComponent<TransformComponent3D>(m_cameraObject).GetTransform());

	DeferredPushConstants modelPushConstants{};

	commandList->BindMaterial(*m_passMaterial);

	// "Caché" de buffers.
	const GpuBuffer* previousVertexBuffer = nullptr;
	const GpuBuffer* previousIndexBuffer = nullptr;

	for (GameObjectIndex obj : objectsToRender) {
		const ModelComponent3D& model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);
		const TransformComponent3D& transform = Engine::GetEcs()->GetComponent<TransformComponent3D>(obj);

		// Actualizamos el modelo 3D, si es necesario.
		if (previousVertexBuffer != &model.GetModel()->GetVertexBuffer()) {
			commandList->BindVertexBuffer(model.GetModel()->GetVertexBuffer());
			previousVertexBuffer = &model.GetModel()->GetVertexBuffer();
		}

		if (previousIndexBuffer != &model.GetModel()->GetIndexBuffer()) {
			commandList->BindIndexBuffer(model.GetModel()->GetIndexBuffer());
			previousIndexBuffer = &model.GetModel()->GetIndexBuffer();
		}

		for (UIndex32 i = 0; i < model.GetModel()->GetMeshes().GetSize(); i++) {
			const auto& mesh = model.GetModel()->GetMeshes()[i];
			const bool isInsideFrustum = mesh.GetBounds().IBroadCollider::IsInsideFrustum(frustum);

			// if (!isInsideFrustum) 
			//	continue;

			auto& modelData = m_localMeshMapping.GetModelData(model.GetModel()->GetUuid());
			const auto& mSlot = *modelData.GetMeshData(mesh.GetUuid()).GetMaterialInstance()->GetSlot("texture");
			commandList->BindMaterialSlot(mSlot);

			modelPushConstants.model = transform.GetTransform().GetAsMatrix();
			modelPushConstants.previousModel = globalMeshMapping->GetPreviousModelMatrix(obj);
			modelPushConstants.resolution = resolution.ToVector2f();
			modelPushConstants.jitterIndex = (float)jitterIndex;

			commandList->PushMaterialConstants("model", modelPushConstants);

			commandList->DrawSingleMesh(
				mesh.GetFirstIndexIdx(), 
				mesh.GetNumIndices());
		}
	}

	commandList->EndDebugSection();
}
