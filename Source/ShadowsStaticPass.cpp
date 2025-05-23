#include "ShadowsStaticPass.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"

#include "CameraComponent3D.h"
#include "TransformComponent3D.h"
#include "ModelComponent3D.h"
#include "Model3D.h"
#include "DeferredPushConstants.h"
#include "Texture.h"

#include "ShadowMap.h"
#include "MatrixOperations.hpp"

#include <unordered_set>

#include "Vertex3D.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


void ShadowsStaticPass::Load() {
	m_passMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/ShadowMapping/material_shadows.json");

	for (auto& mInstance : m_materialInstances) {
		mInstance = m_passMaterial->CreateInstance();
	}
}

void ShadowsStaticPass::SetupShadowMap(const ShadowMap& shadowMap) {
	auto lightUbos = shadowMap.GetGpuBuffers();

	for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		m_materialInstances[i]->GetSlot("global")->SetUniformBuffer("dirLight", *lightUbos[i]);
		m_materialInstances[i]->GetSlot("global")->FlushUpdate();
	}
}

void ShadowsStaticPass::ShadowsRenderLoop(ICommandList* commandList, const DynamicArray<ECS::GameObjectIndex>& objectsToRender, UIndex32 cascadeIndex, const ShadowMap& shadowMap) {
	if (!m_hasBeenSet) {
		SetupShadowMap(shadowMap);
		m_hasBeenSet = true;
	}
	
	commandList->BindMaterial(*m_passMaterial);
	commandList->BindMaterialSlot(*m_materialInstances[Engine::GetRenderer()->GetCurrentResourceIndex()]->GetSlot("global"));

	for (const GameObjectIndex obj : objectsToRender) {
		const ModelComponent3D& model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);
		const auto& transform = Engine::GetEcs()->GetComponent<TransformComponent3D>(obj).GetTransform();

		if (!model.CastsShadows())
			continue;

		commandList->BindVertexBuffer(model.GetModel()->GetVertexBuffer());
		commandList->BindIndexBuffer(model.GetModel()->GetIndexBuffer());

		struct {
			glm::mat4 model;
			int cascadeIndex;
		} const pushConstant{
			.model = transform.GetAsMatrix(),
			.cascadeIndex = static_cast<int>(cascadeIndex)
		};
		commandList->PushMaterialConstants("model", pushConstant);

		for (UIndex32 i = 0; i < model.GetModel()->GetMeshes().GetSize(); i++) {
			commandList->DrawSingleMesh(
				model.GetModel()->GetMeshes()[i].GetFirstIndexIdx(),
				model.GetModel()->GetMeshes()[i].GetNumIndices());
		}
	}
}
