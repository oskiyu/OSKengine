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
#include "IrradianceMap.h"
#include "CameraComponent3D.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::GRAPHICS;

RenderSystem3D::RenderSystem3D() {// Signature del sistema
	Signature signature{};
	signature.SetTrue(Engine::GetEntityComponentSystem()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEntityComponentSystem()->GetComponentType<ModelComponent3D>());
	SetSignature(signature);

	// Mapa de sombras
	shadowMap.Create({ 4096u });

	// Directional light por defecto
	const Vector3f direction = Vector3f(1.0f, -3.f, 0.0f).GetNormalized();
	dirLight.directionAndIntensity = Vector4f(direction.X, direction.Y, direction.Z, 1.2f);
	dirLight.color = Color(253 / 255.f, 253 / 255.f, 225 / 255.f);

	// Material de la escena
	sceneMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_pbr.json");
	sceneMaterialInstance = sceneMaterial->CreateInstance().GetPointer();

	const IGpuUniformBuffer* _cameraUbos[3]{};
	const IGpuUniformBuffer* _dirLightUbos[3]{};
	const IGpuUniformBuffer* _shadowsMatricesUbos[3]{};
	const GpuImage* _shadowsMaps[3]{};
	for (TSize i = 0; i < 3; i++) {
		cameraUbos[i] = Engine::GetRenderer()->GetMemoryAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2 + sizeof(glm::vec4)).GetPointer();
		_cameraUbos[i] = cameraUbos[i].GetPointer();

		dirLightUbos[i] = Engine::GetRenderer()->GetMemoryAllocator()->CreateUniformBuffer(sizeof(DirectionalLight)).GetPointer();
		_dirLightUbos[i] = dirLightUbos[i].GetPointer();

		_shadowsMatricesUbos[i] = shadowMap.GetDirLightMatrixUniformBuffers()[i];
		_shadowsMaps[i] = shadowMap.GetShadowImage(i);
	}

	sceneMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
	sceneMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLight", _dirLightUbos);
	sceneMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLightShadowMat", _shadowsMatricesUbos);
	sceneMaterialInstance->GetSlot("global")->SetGpuImages("dirLightShadowMap", _shadowsMaps, SampledChannel::DEPTH, SampledArrayType::ARRAY);

}

void RenderSystem3D::Initialize(GameObjectIndex camera, const ASSETS::IrradianceMap& irradianceMap) {
	cameraObject = camera;

	sceneMaterialInstance->GetSlot("global")->SetGpuImage("irradianceMap", irradianceMap.GetGpuImage());
	sceneMaterialInstance->GetSlot("global")->FlushUpdate();

	shadowMap.SetSceneCamera(camera);
}

void RenderSystem3D::CreateTargetImage(const Vector2ui& size) {
	renderTarget.Create(size, Format::RGBA32_SFLOAT, Format::D32S8_SFLOAT_SUINT);

	renderTarget.SetName("RenderSystem3D Target");
}

void RenderSystem3D::Resize(const Vector2ui& size) {
	IRenderSystem::Resize(size);
}

ShadowMap* RenderSystem3D::GetShadowMap() {
	return &shadowMap;
}

void RenderSystem3D::GenerateShadows(ICommandList* commandList) {
	const TSize currentFrameIndex = Engine::GetRenderer()->GetCurrentCommandListIndex();
	const Viewport viewport {
		.rectangle = { 0u, 0u, shadowMap.GetColorImage(0)->GetSize().X, shadowMap.GetColorImage(0)->GetSize().Y }
	};

	commandList->SetGpuImageBarrier(shadowMap.GetShadowImage(currentFrameIndex), GpuImageLayout::DEPTH_STENCIL_TARGET,
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::DEPTH_STENCIL_START, GpuBarrierAccessStage::DEPTH_STENCIL_READ | GpuBarrierAccessStage::DEPTH_STENCIL_WRITE),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = shadowMap.GetNumCascades(), .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS, .channel = SampledChannel::DEPTH | SampledChannel::STENCIL });

	commandList->SetViewport(viewport);
	commandList->SetScissor(viewport.rectangle);

	for (TSize i = 0; i < shadowMap.GetNumCascades(); i++) {
		RenderPassImageInfo colorInfo{};
		colorInfo.arrayLevel = i;
		colorInfo.targetImage = shadowMap.GetColorImage(currentFrameIndex);

		RenderPassImageInfo depthInfo{};
		depthInfo.arrayLevel = i;
		depthInfo.targetImage = shadowMap.GetShadowImage(currentFrameIndex);

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

	commandList->SetGpuImageBarrier(shadowMap.GetShadowImage(currentFrameIndex), GpuImageLayout::SHADER_READ_ONLY,
		GpuBarrierInfo(GpuBarrierStage::DEPTH_STENCIL_END, GpuBarrierAccessStage::DEPTH_STENCIL_READ | GpuBarrierAccessStage::DEPTH_STENCIL_WRITE), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = shadowMap.GetNumCascades(), .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS, .channel = SampledChannel::DEPTH |SampledChannel::STENCIL });
}

void RenderSystem3D::RenderScene(GRAPHICS::ICommandList* commandList) {
	const TSize currentFrameIndex = Engine::GetRenderer()->GetCurrentCommandListIndex();
	
	commandList->SetGpuImageBarrier(renderTarget.GetMainTargetImage(currentFrameIndex), GpuImageLayout::SHADER_READ_ONLY,
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT, GpuBarrierAccessStage::COLOR_ATTACHMENT_WRITE),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	IGpuVertexBuffer* previousVertexBuffer = nullptr;
	IGpuIndexBuffer* previousIndexBuffer = nullptr;

	commandList->BeginGraphicsRenderpass(&renderTarget, Color::BLACK() * 0.0f);
	SetupViewport(commandList);
	commandList->BindMaterial(sceneMaterial);
	commandList->BindMaterialSlot(sceneMaterialInstance->GetSlot("global"));

	for (GameObjectIndex obj : GetObjects()) {
		const ModelComponent3D& model = Engine::GetEntityComponentSystem()->GetComponent<ModelComponent3D>(obj);
		const Transform3D& transform = Engine::GetEntityComponentSystem()->GetComponent<Transform3D>(obj);

		if (previousVertexBuffer != model.GetModel()->GetVertexBuffer()) {
			commandList->BindVertexBuffer(model.GetModel()->GetVertexBuffer());
			previousVertexBuffer = model.GetModel()->GetVertexBuffer();
		}
		if (previousIndexBuffer != model.GetModel()->GetIndexBuffer()) {
			commandList->BindIndexBuffer(model.GetModel()->GetIndexBuffer());
			previousIndexBuffer = model.GetModel()->GetIndexBuffer();
		}

		struct {
			glm::mat4 model;
			glm::mat4 transposedInverseModel;
		} const modelConsts {
			.model = transform.GetAsMatrix(),
			.transposedInverseModel = glm::transpose(glm::inverse(transform.GetAsMatrix()))
		};
		commandList->PushMaterialConstants("model", modelConsts);

		for (TSize i = 0; i < model.GetModel()->GetMeshes().GetSize(); i++) {
			commandList->BindMaterialSlot(model.GetMeshMaterialInstance(i)->GetSlot("texture"));

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
	const TSize currentIndex = Engine::GetRenderer()->GetCurrentCommandListIndex();

	const CameraComponent3D& camera = Engine::GetEntityComponentSystem()->GetComponent<CameraComponent3D>(cameraObject);
	const Transform3D& cameraTransform = Engine::GetEntityComponentSystem()->GetComponent<Transform3D>(cameraObject);

	cameraUbos[currentIndex]->MapMemory();
	cameraUbos[currentIndex]->Write(camera.GetProjectionMatrix());
	cameraUbos[currentIndex]->Write(camera.GetViewMatrix(cameraTransform));
	cameraUbos[currentIndex]->Write(cameraTransform.GetPosition());
	cameraUbos[currentIndex]->Unmap();

	dirLightUbos[currentIndex]->MapMemory();
	dirLightUbos[currentIndex]->Write(dirLight);
	dirLightUbos[currentIndex]->Unmap();

	shadowMap.SetDirectionalLight(dirLight);
	
	GenerateShadows(commandList);
	RenderScene(commandList);
}
