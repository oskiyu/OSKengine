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

	// Bloom
	bloomMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_bloom.json");
	for (TSize i = 0; i < _countof(bloomMaterialInstances); i++)
		bloomMaterialInstances[i] = bloomMaterial->CreateInstance().GetPointer();

	bloomResolveMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_bloom_resolve.json");
	bloomResolveMaterialInstance = bloomResolveMaterial->CreateInstance().GetPointer();
}

void RenderSystem3D::CreateTargetImage(const Vector2ui& size) {
	renderTarget.Create(size, Format::RGBA32_SFLOAT, Format::D32S8_SFLOAT_SUINT);
	renderTarget.AddColorTarget(Format::RGBA8_UNORM); // Color brightness para BLOOM

	for (TSize i = 0; i < _countof(bloomRenderTarget); i++)
		bloomRenderTarget[i].Create(size, Format::RGBA8_UNORM, Format::D32S8_SFLOAT_SUINT);

	bloomResolveRenderTarget.Create(size, Format::RGBA8_UNORM, Format::D32S8_SFLOAT_SUINT);

	SetupBloomMaterialSlots();
}

void RenderSystem3D::Resize(const Vector2ui& size) {
	IRenderSystem::Resize(size);

	for (TSize i = 0; i < _countof(bloomRenderTarget); i++)
		bloomRenderTarget[i].Resize(size);

	bloomResolveRenderTarget.Resize(size);

	SetupBloomMaterialSlots();
}

void RenderSystem3D::SetupBloomMaterialSlots() {
	const GpuImage* images[NUM_RESOURCES_IN_FLIGHT]{};

	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = renderTarget.GetTargetImages(i)[1];

	bloomMaterialInstances[0]->GetSlot("texture")->SetGpuImages("bloomTexture", images);
	bloomMaterialInstances[0]->GetSlot("texture")->FlushUpdate();

	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = bloomRenderTarget[0].GetMainTargetImage(i);

	bloomMaterialInstances[1]->GetSlot("texture")->SetGpuImages("bloomTexture", images);
	bloomMaterialInstances[1]->GetSlot("texture")->FlushUpdate();

	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = bloomRenderTarget[1].GetMainTargetImage(i);

	bloomMaterialInstances[2]->GetSlot("texture")->SetGpuImages("bloomTexture", images);
	bloomMaterialInstances[2]->GetSlot("texture")->FlushUpdate();

	// Resolve
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = renderTarget.GetMainTargetImage(i);

	bloomResolveMaterialInstance->GetSlot("texture")->SetGpuImages("sceneTexture", images);

	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = bloomRenderTarget[1].GetMainTargetImage(i);

	bloomResolveMaterialInstance->GetSlot("texture")->SetGpuImages("bloomTexture", images);
	bloomResolveMaterialInstance->GetSlot("texture")->FlushUpdate();
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
	return bloomResolveRenderTarget;
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
	commandList->TransitionImageLayout(renderTarget.GetTargetImages(Engine::GetRenderer()->GetCurrentFrameIndex()).At(1), GpuImageLayout::SHADER_READ_ONLY, GpuImageLayout::COLOR_ATTACHMENT, 0, 1);

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

void RenderSystem3D::BlurBloom(ICommandList* commandList) {
	commandList->BindVertexBuffer(Sprite::globalVertexBuffer);
	commandList->BindIndexBuffer(Sprite::globalIndexBuffer);

	struct {
		glm::mat4 camera;
		glm::mat4 sprite;
		int isHorizontal;
	} pushConst {
		.camera = glm::ortho<float>(0.0f, (float)Engine::GetWindow()->GetWindowSize().X, (float)Engine::GetWindow()->GetWindowSize().Y, 0.0f, -1.0f, 1.0f),
		.sprite = glm::scale(glm::mat4(1.0f), glm::vec3((float)Engine::GetWindow()->GetWindowSize().X, (float)Engine::GetWindow()->GetWindowSize().Y, 1.0f))
	};

	for (TSize i = 0; i < 10; i++) {
		const TSize renderTargetIndex = i % 2;
		const TSize renderTargetInputIndex = 1 - renderTargetIndex;

		const IMaterialSlot* textureSlot = i == 0
			? bloomMaterialInstances[0]->GetSlot("texture")
			: bloomMaterialInstances[renderTargetInputIndex + 1]->GetSlot("texture");

		commandList->BeginGraphicsRenderpass(&bloomRenderTarget[renderTargetIndex], Color::BLACK() * 0.0f);
		SetupViewport(commandList);
		commandList->BindMaterial(bloomMaterial);
		commandList->BindMaterialSlot(textureSlot);

		pushConst.isHorizontal = static_cast<float>(renderTargetIndex);
		commandList->PushMaterialConstants("camera", pushConst);

		commandList->DrawSingleInstance(6);

		commandList->EndGraphicsRenderpass();
	}

	// Resolve
	commandList->BeginGraphicsRenderpass(&bloomResolveRenderTarget, Color::BLACK() * 0.0f);
	SetupViewport(commandList);
	commandList->BindMaterial(bloomResolveMaterial);
	commandList->BindMaterialSlot(bloomResolveMaterialInstance->GetSlot("texture"));

	struct {
		glm::mat4 camera;
		glm::mat4 sprite;
	} pushConst2 {
		.camera = glm::ortho<float>(0.0f, (float)Engine::GetWindow()->GetWindowSize().X, (float)Engine::GetWindow()->GetWindowSize().Y, 0.0f, -1.0f, 1.0f),
		.sprite = glm::scale(glm::mat4(1.0f), glm::vec3((float)Engine::GetWindow()->GetWindowSize().X, (float)Engine::GetWindow()->GetWindowSize().Y, 1.0f))
	};

	commandList->PushMaterialConstants("camera", pushConst2);

	commandList->DrawSingleInstance(6);

	commandList->EndGraphicsRenderpass();
}

void RenderSystem3D::Render(GRAPHICS::ICommandList* commandList) {
	GenerateShadows(commandList);
	RenderScene(commandList);
	BlurBloom(commandList);
}
