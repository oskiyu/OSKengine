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
#include "IGpuImage.h"
#include "GpuImageDimensions.h"
#include "GpuMemoryTypes.h"
#include "GpuImageSamplerDesc.h"

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

	// Bloom
	bloomPass = new BloomPass();
}

void RenderSystem3D::CreateTargetImage(const Vector2ui& size) {
	renderTarget.Create(size, Format::RGBA32_SFLOAT, Format::D32S8_SFLOAT_SUINT);
	renderTarget.AddColorTarget(Format::RGBA8_UNORM); // Color brightness para BLOOM

	renderTarget.SetName("RenderSystem3D Target");

	bloomPass->Create(size);
	bloomPass->SetInput(renderTarget);
}

void RenderSystem3D::Resize(const Vector2ui& size) {
	IRenderSystem::Resize(size);

	bloomPass->Resize(size);
	bloomPass->SetInput(renderTarget);
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

const RenderTarget& RenderSystem3D::GetBloomRenderTarget() const {
	return bloomPass->GetRenderTarget();
}

void RenderSystem3D::GenerateShadows(ICommandList* commandList) {
	const Viewport viewport {
		.rectangle = { 0u, 0u, shadowMap.GetColorImage(0)->GetSize().X, shadowMap.GetColorImage(0)->GetSize().Y }
	};

	commandList->SetGpuImageBarrier(shadowMap.GetShadowImage(Engine::GetRenderer()->GetCurrentFrameIndex()), GpuImageLayout::DEPTH_STENCIL_TARGET,
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::DEPTH_STENCIL_START, GpuBarrierAccessStage::DEPTH_STENCIL_READ | GpuBarrierAccessStage::DEPTH_STENCIL_WRITE),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = shadowMap.GetNumCascades(), .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS, .channel = SampledChannel::DEPTH | SampledChannel::STENCIL });

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

	commandList->SetGpuImageBarrier(shadowMap.GetShadowImage(Engine::GetRenderer()->GetCurrentFrameIndex()), GpuImageLayout::SHADER_READ_ONLY,
		GpuBarrierInfo(GpuBarrierStage::DEPTH_STENCIL_END, GpuBarrierAccessStage::DEPTH_STENCIL_READ | GpuBarrierAccessStage::DEPTH_STENCIL_WRITE), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = shadowMap.GetNumCascades(), .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS, .channel = SampledChannel::DEPTH |SampledChannel::STENCIL });
}

void RenderSystem3D::RenderScene(GRAPHICS::ICommandList* commandList) {
	commandList->SetGpuImageBarrier(renderTarget.GetTargetImages(Engine::GetRenderer()->GetCurrentFrameIndex()).At(1), GpuImageLayout::SHADER_READ_ONLY,
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT, GpuBarrierAccessStage::COLOR_ATTACHMENT_WRITE),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	Material* previousMaterial = nullptr;
	IGpuVertexBuffer* previousVertexBuffer = nullptr;
	IGpuIndexBuffer* previousIndexBuffer = nullptr;

	commandList->BeginGraphicsRenderpass(&renderTarget, Color::BLACK() * 0.0f);
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

			const Vector4f materialInfo {
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
	bloomPass->Execute(Engine::GetRenderer()->GetPreComputeCommandList());
}
