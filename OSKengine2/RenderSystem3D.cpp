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
#include "Model3D.h"
#include "Texture.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

RenderSystem3D::RenderSystem3D() {// Signature del sistema
	Signature signature{};
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<ModelComponent3D>());
	SetSignature(signature);

	// Mapa de sombras
	shadowMap.Create({ 4096u });

	// Directional light por defecto
	const Vector3f direction = Vector3f(1.0f, -3.f, 0.0f).GetNormalized();
	dirLight.directionAndIntensity = Vector4f(direction.X, direction.Y, direction.Z, 1.2f);
	dirLight.color = Color(253 / 255.f, 253 / 255.f, 225 / 255.f);

	// Material del terreno
	terrainMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/terrain.json");
	terrain.SetMaterialInstance(terrainMaterial->CreateInstance());

	// Material de la escena
	sceneMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_pbr.json");
	sceneMaterialInstance = sceneMaterial->CreateInstance().GetPointer();
	animatedSceneMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/Animated/material_pbr.json");

	const IGpuUniformBuffer* _cameraUbos[3]{};
	const IGpuUniformBuffer* _dirLightUbos[3]{};
	const IGpuUniformBuffer* _shadowsMatricesUbos[3]{};
	const GpuImage* _shadowsMaps[3]{};
	for (TSize i = 0; i < 3; i++) {
		cameraUbos[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2 + sizeof(glm::vec4)).GetPointer();
		_cameraUbos[i] = cameraUbos[i].GetPointer();

		dirLightUbos[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(DirectionalLight)).GetPointer();
		_dirLightUbos[i] = dirLightUbos[i].GetPointer();

		_shadowsMatricesUbos[i] = shadowMap.GetDirLightMatrixUniformBuffers()[i];
		_shadowsMaps[i] = shadowMap.GetShadowImage(i);
	}

	sceneMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
	sceneMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLight", _dirLightUbos);
	sceneMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLightShadowMat", _shadowsMatricesUbos);
	sceneMaterialInstance->GetSlot("global")->SetGpuImages("dirLightShadowMap", _shadowsMaps, SampledChannel::DEPTH, SampledArrayType::ARRAY);

}

void RenderSystem3D::Initialize(GameObjectIndex camera, const IrradianceMap& irradianceMap) {
	cameraObject = camera;

	sceneMaterialInstance->GetSlot("global")->SetGpuImage("irradianceMap", irradianceMap.GetGpuImage());
	sceneMaterialInstance->GetSlot("global")->FlushUpdate();

	terrain.GetMaterialInstance()->GetSlot("global")->SetGpuImage("irradianceMap", irradianceMap.GetGpuImage());
	terrain.GetMaterialInstance()->GetSlot("global")->FlushUpdate();

	shadowMap.SetSceneCamera(camera);
}

void RenderSystem3D::InitializeTerrain(const Vector2ui& resolution, const Texture& heightMap, const Texture& texture) {
	terrain.Generate(resolution);

	terrain.GetMaterialInstance()->GetSlot("global")->SetGpuImage("heightmap", heightMap.GetGpuImage());

	const IGpuUniformBuffer* ubos[3]{};
	for (TSize i = 0; i < 3; i++) ubos[i] = cameraUbos[i].GetPointer();
	terrain.GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("camera", ubos);

	for (TSize i = 0; i < 3; i++) ubos[i] = dirLightUbos[i].GetPointer();
	terrain.GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("dirLight", ubos);

	for (TSize i = 0; i < 3; i++) ubos[i] = shadowMap.GetDirLightMatrixUniformBuffers()[i];
	terrain.GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("dirLightShadowMat", ubos);

	const GpuImage* shadowMaps[3]{ shadowMap.GetShadowImage(0), shadowMap.GetShadowImage(1), shadowMap.GetShadowImage(2) };
	terrain.GetMaterialInstance()->GetSlot("global")->SetGpuImages("dirLightShadowMap", shadowMaps, SampledChannel::DEPTH, SampledArrayType::ARRAY);
	terrain.GetMaterialInstance()->GetSlot("global")->FlushUpdate();

	// Albedo
	terrain.GetMaterialInstance()->GetSlot("texture")->SetGpuImage("albedoTexture", texture.GetGpuImage());
	terrain.GetMaterialInstance()->GetSlot("texture")->FlushUpdate();
}

void RenderSystem3D::CreateTargetImage(const Vector2ui& size) {
	RenderTargetAttachmentInfo colorInfo{ .format = Format::RGBA32_SFLOAT, .name = "RenderSystem3D Target" };
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D32S8_SFLOAT_SUINT, .name = "RenderSystem3D Depth" };
	renderTarget.Create(size, { colorInfo }, depthInfo);
}

void RenderSystem3D::Resize(const Vector2ui& size) {
	IRenderSystem::Resize(size);
}

ShadowMap* RenderSystem3D::GetShadowMap() {
	return &shadowMap;
}

void RenderSystem3D::GenerateShadows(ICommandList* commandList, ModelType modelType) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	const Viewport viewport {
		.rectangle = { 0u, 0u, shadowMap.GetColorImage(0)->GetSize().X, shadowMap.GetColorImage(0)->GetSize().Y }
	};

	commandList->SetGpuImageBarrier(shadowMap.GetShadowImage(resourceIndex), GpuImageLayout::DEPTH_STENCIL_TARGET,
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::DEPTH_STENCIL_START, GpuBarrierAccessStage::DEPTH_STENCIL_READ | GpuBarrierAccessStage::DEPTH_STENCIL_WRITE),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = shadowMap.GetNumCascades(), .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS, .channel = SampledChannel::DEPTH | SampledChannel::STENCIL });

	commandList->SetViewport(viewport);
	commandList->SetScissor(viewport.rectangle);

	for (TSize i = 0; i < shadowMap.GetNumCascades(); i++) {
		RenderPassImageInfo colorInfo{};
		colorInfo.arrayLevel = i;
		colorInfo.targetImage = shadowMap.GetColorImage(resourceIndex);

		RenderPassImageInfo depthInfo{};
		depthInfo.arrayLevel = i;
		depthInfo.targetImage = shadowMap.GetShadowImage(resourceIndex);

		commandList->BeginGraphicsRenderpass({ colorInfo }, depthInfo, { 1.0f, 1.0f, 1.0f, 1.0f });

		commandList->BindMaterial(shadowMap.GetShadowsMaterial(ModelType::STATIC_MESH));
		commandList->BindMaterialSlot(shadowMap.GetShadowsMaterialInstance()->GetSlot("global"));

		ShadowsRenderLoop(ModelType::STATIC_MESH, commandList, i);

		commandList->BindMaterial(shadowMap.GetShadowsMaterial(ModelType::ANIMATED_MODEL));
		commandList->BindMaterialSlot(shadowMap.GetShadowsMaterialInstance()->GetSlot("global"));
		ShadowsRenderLoop(ModelType::ANIMATED_MODEL, commandList, i);

		commandList->EndGraphicsRenderpass();
	}

	commandList->SetGpuImageBarrier(shadowMap.GetShadowImage(resourceIndex), GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::DEPTH_STENCIL_END, GpuBarrierAccessStage::DEPTH_STENCIL_READ | GpuBarrierAccessStage::DEPTH_STENCIL_WRITE), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = shadowMap.GetNumCascades(), .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS, .channel = SampledChannel::DEPTH |SampledChannel::STENCIL });
}

void RenderSystem3D::RenderScene(ICommandList* commandList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	
	commandList->SetGpuImageBarrier(renderTarget.GetMainColorImage(resourceIndex), GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT, GpuBarrierAccessStage::COLOR_ATTACHMENT_WRITE),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	commandList->BeginGraphicsRenderpass(&renderTarget, Color::BLACK() * 0.0f);
	SetupViewport(commandList);

	SceneRenderLoop(ModelType::STATIC_MESH, commandList);
	SceneRenderLoop(ModelType::ANIMATED_MODEL, commandList);

	if (terrain.GetVertexBuffer() != nullptr)
		RenderTerrain(commandList);
	
	commandList->EndGraphicsRenderpass();
}

void RenderSystem3D::SceneRenderLoop(ModelType modelType, ICommandList* commandList) {
	IGpuVertexBuffer* previousVertexBuffer = nullptr;
	IGpuIndexBuffer* previousIndexBuffer = nullptr;

	commandList->BindMaterial(modelType == ModelType::STATIC_MESH ? sceneMaterial : animatedSceneMaterial);
	commandList->BindMaterialSlot(sceneMaterialInstance->GetSlot("global"));
	
	for (GameObjectIndex obj : GetObjects()) {
		const ModelComponent3D& model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);
		const Transform3D& transform = Engine::GetEcs()->GetComponent<Transform3D>(obj);

		if (model.GetModel()->GetType() != modelType)
			continue;

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
			glm::vec4 materialInfos;
		} pushConsts{
			.model = transform.GetAsMatrix(),
			.transposedInverseModel = glm::transpose(glm::inverse(transform.GetAsMatrix()))
		};

		if (modelType == ModelType::ANIMATED_MODEL)
			commandList->BindMaterialSlot(model.GetModel()->GetAnimator()->GetMaterialInstance()->GetSlot("animation"));

		for (TSize i = 0; i < model.GetModel()->GetMeshes().GetSize(); i++) {
			commandList->BindMaterialSlot(model.GetMeshMaterialInstance(i)->GetSlot("texture"));

			pushConsts.materialInfos.x = model.GetModel()->GetMetadata().meshesMetadata[i].metallicFactor;
			pushConsts.materialInfos.y = model.GetModel()->GetMetadata().meshesMetadata[i].roughnessFactor;

			commandList->PushMaterialConstants("pushConstants", pushConsts);

			commandList->DrawSingleMesh(model.GetModel()->GetMeshes()[i].GetFirstIndexId(), model.GetModel()->GetMeshes()[i].GetNumberOfIndices());
		}
	}
}

void RenderSystem3D::ShadowsRenderLoop(ModelType modelType, ICommandList* commandList, TSize cascadeIndex) {
	for (const GameObjectIndex obj : GetObjects()) {
		const ModelComponent3D& model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);
		const Transform3D& transform = Engine::GetEcs()->GetComponent<Transform3D>(obj);

		if (model.GetModel()->GetType() != modelType)
			continue;

		commandList->BindVertexBuffer(model.GetModel()->GetVertexBuffer());
		commandList->BindIndexBuffer(model.GetModel()->GetIndexBuffer());

		if (modelType == ModelType::ANIMATED_MODEL)
			commandList->BindMaterialSlot(model.GetModel()->GetAnimator()->GetMaterialInstance()->GetSlot("animation"));

		struct {
			glm::mat4 model;
			int cascadeIndex;
		} const pushConstant{
			.model = transform.GetAsMatrix(),
			.cascadeIndex = static_cast<int>(cascadeIndex)
		};
		commandList->PushMaterialConstants("model", pushConstant);

		for (TSize i = 0; i < model.GetModel()->GetMeshes().GetSize(); i++)
			commandList->DrawSingleMesh(model.GetModel()->GetMeshes()[i].GetFirstIndexId(), model.GetModel()->GetMeshes()[i].GetNumberOfIndices());
	}
}

void RenderSystem3D::RenderTerrain(ICommandList* commandList) {
	commandList->BindMaterial(terrainMaterial);
	commandList->BindMaterialSlot(terrain.GetMaterialInstance()->GetSlot("global"));
	commandList->BindMaterialSlot(terrain.GetMaterialInstance()->GetSlot("texture"));

	commandList->BindVertexBuffer(terrain.GetVertexBuffer());
	commandList->BindIndexBuffer(terrain.GetIndexBuffer());
	
	glm::mat4 model = glm::scale(glm::mat4(1.0f), { 100.f, 5.f, 100.f });

	struct {
		glm::mat4 model;
		glm::mat4 transposedInverseModel;
		glm::vec4 materialInfos;
	} pushConsts{
		.model = model,
		.transposedInverseModel = glm::transpose(glm::inverse(model)),
		.materialInfos = { 0.5f, 0.5f, 0.0f, 0.0f }
	};
	commandList->PushMaterialConstants("pushConstants", pushConsts);
	
	commandList->DrawSingleMesh(0, terrain.GetNumIndices());
}

void RenderSystem3D::Render(GRAPHICS::ICommandList* commandList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	const CameraComponent3D& camera = Engine::GetEcs()->GetComponent<CameraComponent3D>(cameraObject);
	const Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(cameraObject);

	cameraUbos[resourceIndex]->MapMemory();
	cameraUbos[resourceIndex]->Write(camera.GetProjectionMatrix());
	cameraUbos[resourceIndex]->Write(camera.GetViewMatrix(cameraTransform));
	cameraUbos[resourceIndex]->Write(cameraTransform.GetPosition());
	cameraUbos[resourceIndex]->Unmap();

	dirLightUbos[resourceIndex]->MapMemory();
	dirLightUbos[resourceIndex]->Write(dirLight);
	dirLightUbos[resourceIndex]->Unmap();

	shadowMap.SetDirectionalLight(dirLight);
	
	GenerateShadows(commandList, ModelType::STATIC_MESH);
	RenderScene(commandList);
}

void RenderSystem3D::OnTick(TDeltaTime deltaTime) {
	for (const GameObjectIndex obj : GetObjects()) {
		Model3D* model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj).GetModel();

		if (model->GetType() == ModelType::STATIC_MESH)
			continue;

		model->GetAnimator()->Update(deltaTime);
	}
}
