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
#include "CubemapTexture.h"
#include "CameraComponent3D.h"
#include "Model3D.h"
#include "Texture.h"
#include "SpecularMap.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

RenderSystem3D::RenderSystem3D() {// Signature del sistema
	Signature signature{};
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<ModelComponent3D>());
	_SetSignature(signature);


	// Mapa de sombras
	// Before: 2048MB -> 2.048 GB, 0.78ms
	// After: 1210MB -> 1.21 GB, 0.63ms
	shadowMap.Create({ 4096u });

	// Directional light por defecto
	const Vector3f direction = Vector3f(1.0f, -1.9f, 0.0f).GetNormalized();
	dirLight.directionAndIntensity = Vector4f(direction.X, direction.Y, direction.Z, 1.4f);
	dirLight.color = Color(255 / 255.f, 255 / 255.f, 255 / 255.f);

	CreateBuffers();
	LoadMaterials();
	SetupMaterials();	

	taaProvider.SetMaxJitter(4);
}

void RenderSystem3D::Initialize(GameObjectIndex camera, const IrradianceMap& irradianceMap, const SpecularMap& specularMap) {
	cameraObject = camera;

	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_Default();
	const GpuImageViewConfig cubemapView = GpuImageViewConfig::CreateSampled_Cubemap();

	sceneMaterialInstance->GetSlot("global")->SetGpuImage("irradianceMap", irradianceMap.GetGpuImage()->GetView(cubemapView));
	sceneMaterialInstance->GetSlot("global")->SetGpuImage("specularMap", specularMap.GetCubemapImage()->GetView(cubemapView));
	sceneMaterialInstance->GetSlot("global")->SetGpuImage("specularLut", specularMap.GetLookUpTable()->GetView(viewConfig));
	sceneMaterialInstance->GetSlot("global")->FlushUpdate();

	// terrain.GetMaterialInstance()->GetSlot("global")->SetGpuImage("irradianceMap", irradianceMap.GetGpuImage()->GetView(cubemapView));
	// terrain.GetMaterialInstance()->GetSlot("global")->SetGpuImage("specularMap", specularMap.GetGpuImage());
	// terrain.GetMaterialInstance()->GetSlot("global")->FlushUpdate();

	shadowMap.SetSceneCamera(camera);
}

void RenderSystem3D::CreateBuffers() {
	for (TIndex i = 0; i < 3; i++) {
		cameraBuffers[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2 + sizeof(glm::vec4)).GetPointer();
		previousCameraBuffers[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2).GetPointer();

		dirLightUbos[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(DirectionalLight)).GetPointer();
	}

	resolutionBuffer = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(Vector2f)).GetPointer();
}

void RenderSystem3D::LoadMaterials() {
	// Material del terreno
	terrainMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/terrain_direct_pbr.json");
	terrain.SetMaterialInstance(terrainMaterial->CreateInstance());

	// Materiales de la escena
	sceneMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/direct_pbr.json");
	sceneMaterialInstance = sceneMaterial->CreateInstance().GetPointer();
	animatedSceneMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/animated_direct_pbr.json");
}

void RenderSystem3D::SetupMaterials() {
	const GpuBuffer* _cameraUbos[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuBuffer* _previousCameraUbos[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuBuffer* _dirLightUbos[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuBuffer* _shadowsMatricesUbos[NUM_RESOURCES_IN_FLIGHT]{};
	const IGpuImageView* _shadowsMaps[NUM_RESOURCES_IN_FLIGHT]{};

	GpuImageViewConfig shadowsViewConfig = GpuImageViewConfig::CreateSampled_Array(0, shadowMap.GetNumCascades());
	shadowsViewConfig.channel = SampledChannel::DEPTH;

	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		_cameraUbos[i] = cameraBuffers[i].GetPointer();
		_previousCameraUbos[i] = previousCameraBuffers[i].GetPointer();
		_dirLightUbos[i] = dirLightUbos[i].GetPointer();
		_shadowsMatricesUbos[i] = shadowMap.GetDirLightMatrixUniformBuffers()[i];
		_shadowsMaps[i] = shadowMap.GetShadowImage(i)->GetView(shadowsViewConfig);
	}

	sceneMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
	sceneMaterialInstance->GetSlot("global")->SetUniformBuffers("previousCamera", _previousCameraUbos);
	sceneMaterialInstance->GetSlot("global")->SetUniformBuffer("res", resolutionBuffer.GetValue());

	sceneMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLight", _dirLightUbos);
	sceneMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLightShadowMat", _shadowsMatricesUbos);
	sceneMaterialInstance->GetSlot("global")->SetGpuImages("dirLightShadowMap", _shadowsMaps);

	sceneMaterialInstance->GetSlot("global")->FlushUpdate(); // No está completo.
}

void RenderSystem3D::InitializeTerrain(const Vector2ui& resolution, const Texture& heightMap, const Texture& texture) {
	return;
	
	terrain.Generate(resolution);

	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_MipLevelRanged(0, 0);
	terrain.GetMaterialInstance()->GetSlot("global")->SetGpuImage("heightmap", heightMap.GetGpuImage()->GetView(viewConfig));

	const GpuBuffer* ubos[3]{};
	for (TSize i = 0; i < 3; i++) ubos[i] = cameraBuffers[i].GetPointer();
	terrain.GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("camera", ubos);

	for (TSize i = 0; i < 3; i++) ubos[i] = dirLightUbos[i].GetPointer();
	terrain.GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("dirLight", ubos);

	for (TSize i = 0; i < 3; i++) ubos[i] = shadowMap.GetDirLightMatrixUniformBuffers()[i];
	terrain.GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("dirLightShadowMat", ubos);

	GpuImageViewConfig shadowsViewConfig = GpuImageViewConfig::CreateSampled_Array(0, shadowMap.GetNumCascades());
	shadowsViewConfig.channel = SampledChannel::DEPTH;

	const IGpuImageView* shadowMaps[3] { 
		shadowMap.GetShadowImage(0)->GetView(shadowsViewConfig), 
		shadowMap.GetShadowImage(1)->GetView(shadowsViewConfig), 
		shadowMap.GetShadowImage(2)->GetView(shadowsViewConfig) 
	};

	terrain.GetMaterialInstance()->GetSlot("global")->SetGpuImages("dirLightShadowMap", shadowMaps);
	terrain.GetMaterialInstance()->GetSlot("global")->FlushUpdate();

	// Albedo
	terrain.GetMaterialInstance()->GetSlot("texture")->SetGpuImage("albedoTexture", texture.GetGpuImage()->GetView(GpuImageViewConfig::CreateSampled_Default()));
	terrain.GetMaterialInstance()->GetSlot("texture")->FlushUpdate();
}

void RenderSystem3D::CreateTargetImage(const Vector2ui& size) {
	RenderTargetAttachmentInfo colorInfo{ .format = Format::RGBA16_SFLOAT, .usage = GpuImageUsage::TRANSFER_DESTINATION, .name = "RenderSystem3D Target" };
	RenderTargetAttachmentInfo motionInfo{ .format = Format::RG16_SFLOAT, .name = "RenderSystem3D Motion" };
	RenderTargetAttachmentInfo depthInfo{ .format = Format::D32S8_SFLOAT_SUINT, .name = "RenderSystem3D Depth" };
	renderTarget.Create(size, { colorInfo, motionInfo }, depthInfo);

	// TAA
	const GpuImage* sourceImages[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuImage* motionImages[NUM_RESOURCES_IN_FLIGHT]{};

	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		sourceImages[i] = renderTarget.GetColorImage(COLOR_IMAGE_INDEX, i);
		motionImages[i] = renderTarget.GetColorImage(MOTION_IMAGE_INDEX, i);
	}

	taaProvider.InitializeTaa(
		size,
		sourceImages,
		motionImages);

	resolutionBuffer->MapMemory();
	resolutionBuffer->Write(size.ToVector2f());
	resolutionBuffer->Unmap();
}

void RenderSystem3D::Resize(const Vector2ui& size) {
	IRenderSystem::Resize(size);

	// TAA
	const GpuImage* sourceImages[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuImage* motionImages[NUM_RESOURCES_IN_FLIGHT]{};

	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		sourceImages[i] = renderTarget.GetColorImage(COLOR_IMAGE_INDEX, i);
		motionImages[i] = renderTarget.GetColorImage(MOTION_IMAGE_INDEX, i);
	}

	taaProvider.ResizeTaa(
		size,
		sourceImages,
		motionImages);

	resolutionBuffer->MapMemory();
	resolutionBuffer->Write(size.ToVector2f());
	resolutionBuffer->Unmap();
}

ShadowMap* RenderSystem3D::GetShadowMap() {
	return &shadowMap;
}

void RenderSystem3D::GenerateShadows(ICommandList* commandList, ModelType modelType) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	const Viewport viewport {
		.rectangle = { 0u, 0u, 
		shadowMap.GetColorImage(0)->GetSize2D().X, 
		shadowMap.GetColorImage(0)->GetSize2D().Y }
	};

	commandList->StartDebugSection("Shadows", Color::BLACK());

	commandList->SetGpuImageBarrier(
		shadowMap.GetShadowImage(resourceIndex), 
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::DEPTH_STENCIL_TARGET,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::DEPTH_STENCIL_START, GpuAccessStage::DEPTH_STENCIL_READ | GpuAccessStage::DEPTH_STENCIL_WRITE),
		GpuImageRange{ .baseLayer = 0, .numLayers = shadowMap.GetNumCascades(), .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS, .channel = SampledChannel::DEPTH });

	commandList->SetViewport(viewport);
	commandList->SetScissor(viewport.rectangle);

	for (TSize i = 0; i < shadowMap.GetNumCascades(); i++) {

		commandList->StartDebugSection("Cascade " + std::to_string(i), Color::BLACK());

		RenderPassImageInfo colorInfo{};
		colorInfo.arrayLevel = i;
		colorInfo.targetImage = shadowMap.GetColorImage(resourceIndex);

		RenderPassImageInfo depthInfo{};
		depthInfo.arrayLevel = i;
		depthInfo.targetImage = shadowMap.GetShadowImage(resourceIndex);

		commandList->BeginGraphicsRenderpass({ colorInfo }, depthInfo, { 1.0f, 1.0f, 1.0f, 1.0f }, false);

		commandList->StartDebugSection("Static Meshes", Color::BLACK());

		commandList->BindMaterial(*shadowMap.GetShadowsMaterial(ModelType::STATIC_MESH));
		commandList->BindMaterialSlot(*shadowMap.GetShadowsMaterialInstance()->GetSlot("global"));

		ShadowsRenderLoop(ModelType::STATIC_MESH, commandList, i);

		commandList->EndDebugSection();

		commandList->StartDebugSection("Animated Models", Color::BLACK());

		commandList->BindMaterial(*shadowMap.GetShadowsMaterial(ModelType::ANIMATED_MODEL));
		commandList->BindMaterialSlot(*shadowMap.GetShadowsMaterialInstance()->GetSlot("global"));
		ShadowsRenderLoop(ModelType::ANIMATED_MODEL, commandList, i);

		commandList->EndDebugSection();

		commandList->EndGraphicsRenderpass(false);

		commandList->EndDebugSection();
	}

	commandList->EndDebugSection();
}

void RenderSystem3D::RenderScene(ICommandList* commandList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	
	commandList->StartDebugSection("PBR Direct", Color::RED());

	commandList->SetGpuImageBarrier(shadowMap.GetShadowImage(resourceIndex),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SAMPLED_READ),
		GpuImageRange{
			.baseLayer = 0, 
			.numLayers = shadowMap.GetNumCascades(), 
			.baseMipLevel = 0, 
			.numMipLevels = ALL_MIP_LEVELS, 
			.channel = SampledChannel::DEPTH });

	commandList->BeginGraphicsRenderpass(&renderTarget, Color::BLACK() * 0.0f);
	SetupViewport(commandList);

	commandList->StartDebugSection("Static Meshes", Color::RED());
	SceneRenderLoop(ModelType::STATIC_MESH, commandList);
	commandList->EndDebugSection();

	commandList->StartDebugSection("Animated Models", Color::RED());
	// SceneRenderLoop(ModelType::ANIMATED_MODEL, commandList);
	commandList->EndDebugSection();

	if (terrain.GetVertexBuffer() != nullptr)
		RenderTerrain(commandList);
	
	commandList->EndGraphicsRenderpass();
	commandList->EndDebugSection();
}

void RenderSystem3D::SceneRenderLoop(ModelType modelType, ICommandList* commandList) {
	GpuBuffer* previousVertexBuffer = nullptr;
	GpuBuffer* previousIndexBuffer = nullptr;

	commandList->BindMaterial(modelType == ModelType::STATIC_MESH ? *sceneMaterial : *animatedSceneMaterial);
	commandList->BindMaterialSlot(*sceneMaterialInstance->GetSlot("global"));
	
	for (GameObjectIndex obj : GetObjects()) {
		const ModelComponent3D& model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);
		const Transform3D& transform = Engine::GetEcs()->GetComponent<Transform3D>(obj);

		if (model.GetModel()->GetType() != modelType)
			continue;

		if (previousVertexBuffer != model.GetModel()->GetVertexBuffer()) {
			commandList->BindVertexBuffer(*model.GetModel()->GetVertexBuffer());
			previousVertexBuffer = model.GetModel()->GetVertexBuffer();
		}
		if (previousIndexBuffer != model.GetModel()->GetIndexBuffer()) {
			commandList->BindIndexBuffer(*model.GetModel()->GetIndexBuffer());
			previousIndexBuffer = model.GetModel()->GetIndexBuffer();
		}

		struct {
			glm::mat4 model;
			glm::mat4 previousModel;
			glm::vec4 materialInfos;
		} pushConsts{
			.model = transform.GetAsMatrix(),
			.previousModel = previousModelMatrices.ContainsKey(obj) ? previousModelMatrices.Get(obj) : glm::mat4(1.0f)
		};

		if (modelType == ModelType::ANIMATED_MODEL)
			commandList->BindMaterialSlot(*model.GetModel()->GetAnimator()->GetMaterialInstance()->GetSlot("animation"));

		for (TSize i = 0; i < model.GetModel()->GetMeshes().GetSize(); i++) {
			commandList->BindMaterialSlot(*model.GetMeshMaterialInstance(i)->GetSlot("texture"));

			pushConsts.materialInfos.x = model.GetModel()->GetMetadata().meshesMetadata[i].metallicFactor;
			pushConsts.materialInfos.y = model.GetModel()->GetMetadata().meshesMetadata[i].roughnessFactor;
			pushConsts.materialInfos.z = (float)taaProvider.GetCurrentFrameJitterIndex();

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

		commandList->BindVertexBuffer(*model.GetModel()->GetVertexBuffer());
		commandList->BindIndexBuffer(*model.GetModel()->GetIndexBuffer());

		if (modelType == ModelType::ANIMATED_MODEL)
			commandList->BindMaterialSlot(*model.GetModel()->GetAnimator()->GetMaterialInstance()->GetSlot("animation"));

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
	commandList->BindMaterial(*terrainMaterial);
	commandList->BindMaterialInstance(*terrain.GetMaterialInstance());

	commandList->BindVertexBuffer(*terrain.GetVertexBuffer());
	commandList->BindIndexBuffer(*terrain.GetIndexBuffer());
	
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

void RenderSystem3D::ExecuteTaa(ICommandList* commandList) {
	commandList->StartDebugSection("TAA", Color::YELLOW());

	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	commandList->SetGpuImageBarrier(
		renderTarget.GetColorImage(MOTION_IMAGE_INDEX, resourceIndex),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	commandList->SetGpuImageBarrier(
		renderTarget.GetColorImage(COLOR_IMAGE_INDEX, resourceIndex),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	taaProvider.ExecuteTaa(commandList);

	if (taaProvider.IsActive())
		CopyTaaResult(commandList);
	
	commandList->EndDebugSection();
}

void RenderSystem3D::CopyTaaResult(GRAPHICS::ICommandList* commandList) {
	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	GpuImage* sourceImage = taaProvider.GetTaaOutput().GetTargetImage(resourceIndex);
	GpuImage* destinationImage = renderTarget.GetColorImage(COLOR_IMAGE_INDEX, resourceIndex);

	// Imagen original TAA: transfer source.
	commandList->SetGpuImageBarrier(
		sourceImage,
		GpuImageLayout::TRANSFER_SOURCE,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_READ));

	// Imagen del render target final: transfer destination.
	commandList->SetGpuImageBarrier(
		destinationImage,
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE));

	commandList->RawCopyImageToImage(
		*sourceImage,
		destinationImage,
		CopyImageInfo::CreateDefault2D(renderTarget.GetSize()));
}

void RenderSystem3D::Render(GRAPHICS::ICommandList* commandList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	const CameraComponent3D& camera = Engine::GetEcs()->GetComponent<CameraComponent3D>(cameraObject);
	const Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(cameraObject);

	const glm::mat4 currentProjection = camera.GetProjectionMatrix();
	const glm::mat4 currentView = camera.GetViewMatrix(cameraTransform);

	cameraBuffers[resourceIndex]->MapMemory();
	cameraBuffers[resourceIndex]->Write(currentProjection);
	cameraBuffers[resourceIndex]->Write(currentView);
	cameraBuffers[resourceIndex]->Write(cameraTransform.GetPosition());
	cameraBuffers[resourceIndex]->Unmap();

	previousCameraBuffers[resourceIndex]->MapMemory();
	previousCameraBuffers[resourceIndex]->Write(previousCameraProjection);
	previousCameraBuffers[resourceIndex]->Write(previousCameraView);
	previousCameraBuffers[resourceIndex]->Unmap();

	dirLightUbos[resourceIndex]->MapMemory();
	dirLightUbos[resourceIndex]->Write(dirLight);
	dirLightUbos[resourceIndex]->Unmap();

	previousCameraProjection = currentProjection;
	previousCameraView= currentView;

	shadowMap.SetDirectionalLight(dirLight);
	
	GenerateShadows(commandList, ModelType::STATIC_MESH);
	RenderScene(commandList);
	ExecuteTaa(commandList);

	for (const GameObjectIndex obj : GetObjects())
		previousModelMatrices.Insert(obj, Engine::GetEcs()->GetComponent<Transform3D>(obj).GetAsMatrix());
}

void RenderSystem3D::OnTick(TDeltaTime deltaTime) {
	for (const GameObjectIndex obj : GetObjects()) {
		Model3D* model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj).GetModel();

		if (model->GetType() == ModelType::STATIC_MESH)
			continue;

		model->GetAnimator()->Update(deltaTime);
	}
}
