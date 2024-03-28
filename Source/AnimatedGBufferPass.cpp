#include "AnimatedGBufferPass.h"

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


void AnimatedGBufferPass::Load() {
	m_passMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/Deferred/deferred_gbuffer_anim.json");
}

void AnimatedGBufferPass::RenderLoop(ICommandList* commandList, const DynamicArray<ECS::GameObjectIndex>& objectsToRender, GlobalMeshMapping* meshMapping, UIndex32 jitterIndex, Vector2ui resolution) {
	commandList->StartDebugSection("Animated Pass", Color::Red);
	
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
			const bool isInsideFrustum = mesh.GetBounds().IsInsideFrustum(frustum);

			if (!isInsideFrustum)
				continue;

			auto& modelData = m_localMeshMapping.GetModelData(model.GetModel()->GetUuid());
			const auto& mSlot = *modelData.GetMeshData(mesh.GetUuid()).GetMaterialInstance()->GetSlot("texture");
			commandList->BindMaterialSlot(mSlot);

			commandList->BindMaterialInstance(*meshMapping->GetModelData(model.GetModel()->GetUuid()).GetAnimationMaterialInstance());

			modelPushConstants.model = transform.GetAsMatrix();
			modelPushConstants.previousModel = meshMapping->GetPreviousModelMatrix(obj);
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
