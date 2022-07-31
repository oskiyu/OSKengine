#include "RenderSystem3D.h"

#include "Transform3D.h"
#include "ModelComponent3D.h"
#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "ICommandList.h"
#include "MaterialInstance.h"
#include "Material.h"
#include "MaterialLayout.h"
#include "Model3D.h"
#include "Mesh3D.h"
#include "Viewport.h"
#include "GpuImageLayout.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

RenderSystem3D::RenderSystem3D() {
	Signature signature{};

	signature.SetTrue(Engine::GetEntityComponentSystem()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEntityComponentSystem()->GetComponentType<ModelComponent3D>());

	SetSignature(signature);

	shadowMap.Create({ 2048u, 2048u });

	dirLightUniformBuffer = Engine::GetRenderer()->GetMemoryAllocator()->CreateUniformBuffer(sizeof(DirectionalLight)).GetPointer();
}

void RenderSystem3D::SetDirectionalLight(const DirectionalLight& dirLight) {
	directionalLight = dirLight;

	dirLightUniformBuffer->ResetCursor();
	dirLightUniformBuffer->MapMemory();
	dirLightUniformBuffer->Write(directionalLight);
	dirLightUniformBuffer->Unmap();

	shadowMap.SetDirectionalLight(dirLight);
}

const DirectionalLight& RenderSystem3D::GetDirectionalLight() const {
	return directionalLight;
}

IGpuUniformBuffer* RenderSystem3D::GetDirLightUniformBuffer() const {
	return dirLightUniformBuffer.GetPointer();
}

ShadowMap* RenderSystem3D::GetShadowMap() {
	return &shadowMap;
}

void RenderSystem3D::GenerateShadows(ICommandList* commandList) {
	const Viewport viewport {
		.rectangle = { 0u, 0u, shadowMap.GetColorImage(0)->GetSize().X, shadowMap.GetColorImage(0)->GetSize().Y }
	};

	commandList->TransitionImageLayout(shadowMap.GetShadowImage(Engine::GetRenderer()->GetCurrentFrameIndex()), GpuImageLayout::DEPTH_STENCIL_TARGET, 0, shadowMap.GetNumCascades());

	commandList->SetViewport(viewport);
	commandList->SetScissor(viewport.rectangle);

	for (TSize i = 0; i < shadowMap.GetNumCascades(); i++) {
		RenderPassImageInfo colorInfo{};
		colorInfo.arrayLevel = i;
		colorInfo.targetImage = shadowMap.GetColorImage(Engine::GetRenderer()->GetCurrentFrameIndex());

		RenderPassImageInfo depthInfo{};
		depthInfo.arrayLevel = i;
		depthInfo.targetImage = shadowMap.GetShadowImage(Engine::GetRenderer()->GetCurrentFrameIndex());

		commandList->BeginGraphicsRenderpass({ colorInfo }, depthInfo, { 1.0f, 1.0f, 1.0f, 1.0f });

		commandList->BindMaterial(shadowMap.GetShadowsMaterial());
		commandList->BindMaterialSlot(shadowMap.GetShadowsMaterialInstance()->GetSlot("global"));

		for (const GameObjectIndex obj : GetObjects()) {
			const ModelComponent3D& model = Engine::GetEntityComponentSystem()->GetComponent<ModelComponent3D>(obj);
			const Transform3D& transform = Engine::GetEntityComponentSystem()->GetComponent<Transform3D>(obj);

			commandList->BindVertexBuffer(model.GetModel()->GetVertexBuffer());
			commandList->BindIndexBuffer(model.GetModel()->GetIndexBuffer());

			struct {
				glm::mat4 model;
				int cascadeIndex;
			} const pushConstant {
				.model = transform.GetAsMatrix(),
				.cascadeIndex = static_cast<int>(i)
			};
			commandList->PushMaterialConstants("model", pushConstant);

			for (TSize i = 0; i < model.GetModel()->GetMeshes().GetSize(); i++)
				commandList->DrawSingleMesh(model.GetModel()->GetMeshes()[i].GetFirstIndexId(), model.GetModel()->GetMeshes()[i].GetNumberOfIndices());
		}

		commandList->EndGraphicsRenderpass();
	}

	commandList->TransitionImageLayout(shadowMap.GetShadowImage(Engine::GetRenderer()->GetCurrentFrameIndex()), GpuImageLayout::SHADER_READ_ONLY, 0, shadowMap.GetNumCascades());
}

void RenderSystem3D::RenderScene(GRAPHICS::ICommandList* commandList) {
	Material* previousMaterial = nullptr;
	IGpuVertexBuffer* previousVertexBuffer = nullptr;
	IGpuIndexBuffer* previousIndexBuffer = nullptr;

	commandList->BeginGraphicsRenderpass(&renderTarget, Color::WHITE() * 0.0f);

	SetupViewport(commandList);

	for (GameObjectIndex obj : GetObjects()) {
		const ModelComponent3D& model = Engine::GetEntityComponentSystem()->GetComponent<ModelComponent3D>(obj);
		const Transform3D& transform = Engine::GetEntityComponentSystem()->GetComponent<Transform3D>(obj);

		if (previousMaterial != model.GetMaterial()) {
			commandList->BindMaterial(model.GetMaterial());
			previousMaterial = model.GetMaterial();
		}

		if (previousVertexBuffer != model.GetModel()->GetVertexBuffer()) {
			commandList->BindVertexBuffer(model.GetModel()->GetVertexBuffer());
			previousVertexBuffer = model.GetModel()->GetVertexBuffer();
		}
		if (previousIndexBuffer != model.GetModel()->GetIndexBuffer()) {
			commandList->BindIndexBuffer(model.GetModel()->GetIndexBuffer());
			previousIndexBuffer = model.GetModel()->GetIndexBuffer();
		}

		commandList->PushMaterialConstants("model", transform.GetAsMatrix());

		for (TSize i = 0; i < model.GetModel()->GetMeshes().GetSize(); i++) {
			for (const std::string& slotName : model.GetMaterial()->GetLayout()->GetAllSlotNames())
				commandList->BindMaterialSlot(model.GetMeshMaterialInstance(i)->GetSlot(slotName));

			const Vector4f materialInfo{
				model.GetModel()->GetMetadata().meshesMetadata[i].metallicFactor,
				model.GetModel()->GetMetadata().meshesMetadata[i].roughnessFactor,
				0.0f,
				0.0f
			};
			commandList->PushMaterialConstants("materialInfo", materialInfo);

			commandList->DrawSingleMesh(model.GetModel()->GetMeshes()[i].GetFirstIndexId(), model.GetModel()->GetMeshes()[i].GetNumberOfIndices());
		}
	}

	commandList->EndGraphicsRenderpass();
}

void RenderSystem3D::Render(GRAPHICS::ICommandList* commandList) {
	GenerateShadows(commandList);
	RenderScene(commandList);
}
