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
	m_shadowMap.Create(Vector2ui(4096u));

	// Directional light por defecto
	const Vector3f direction = Vector3f(1.0f, -1.9f, 0.0f).GetNormalized();
	m_dirLight.directionAndIntensity = Vector4f(direction.x, direction.y, direction.z, 1.0f);
	m_dirLight.color = Color(255 / 255.f, 255 / 255.f, 255 / 255.f);

	CreateBuffers();
	LoadMaterials();
	SetupMaterials();	

	m_taaProvider.SetMaxJitter(4);
}

void RenderSystem3D::Initialize(GameObjectIndex camera, const IrradianceMap& irradianceMap, const SpecularMap& specularMap) {
	m_cameraObject = camera;

	const GpuImageViewConfig viewConfig  = GpuImageViewConfig::CreateSampled_Default();
	const GpuImageViewConfig cubemapView = GpuImageViewConfig::CreateSampled_Cubemap();

	m_sceneMaterialInstance->GetSlot("global")->SetGpuImage("irradianceMap", irradianceMap.GetGpuImage()->GetView(cubemapView));
	m_sceneMaterialInstance->GetSlot("global")->SetGpuImage("specularMap", specularMap.GetCubemapImage()->GetView(cubemapView));
	m_sceneMaterialInstance->GetSlot("global")->SetGpuImage("specularLut", specularMap.GetLookUpTable()->GetView(viewConfig));
	m_sceneMaterialInstance->GetSlot("global")->FlushUpdate();

	// terrain.GetMaterialInstance()->GetSlot("global")->SetGpuImage("irradianceMap", irradianceMap.GetGpuImage()->GetView(cubemapView));
	// terrain.GetMaterialInstance()->GetSlot("global")->SetGpuImage("specularMap", specularMap.GetGpuImage());
	// terrain.GetMaterialInstance()->GetSlot("global")->FlushUpdate();

	m_shadowMap.SetSceneCamera(camera);
}

void RenderSystem3D::CreateBuffers() {
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		m_cameraBuffers[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2 + sizeof(glm::vec4)).GetPointer();
		m_previousCameraBuffers[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2).GetPointer();

		m_dirLightUbos[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(DirectionalLight)).GetPointer();
	}

	m_resolutionBuffer = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(Vector2f)).GetPointer();
}

void RenderSystem3D::LoadMaterials() {
	// Material del terreno
	m_terrainMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/terrain_direct_pbr.json");
	m_terrain.SetMaterialInstance(m_terrainMaterial->CreateInstance());

	// Materiales de la escena
	m_sceneMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/direct_pbr.json");
	m_sceneMaterialInstance = m_sceneMaterial->CreateInstance().GetPointer();
	m_animatedSceneMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/animated_direct_pbr.json");
}

void RenderSystem3D::SetupMaterials() {
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> _cameraUbos{};
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> _previousCameraUbos{};
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> _dirLightUbos{};
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> _shadowsMatricesUbos{};
	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> _shadowsMaps{};

	GpuImageViewConfig shadowsViewConfig = GpuImageViewConfig::CreateSampled_Array(0, m_shadowMap.GetNumCascades());
	shadowsViewConfig.channel = SampledChannel::DEPTH;

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		_cameraUbos[i] = m_cameraBuffers[i].GetPointer();
		_previousCameraUbos[i] = m_previousCameraBuffers[i].GetPointer();
		_dirLightUbos[i] = m_dirLightUbos[i].GetPointer();
		_shadowsMatricesUbos[i] = m_shadowMap.GetDirLightMatrixUniformBuffers()[i];
		_shadowsMaps[i] = m_shadowMap.GetShadowImage(i)->GetView(shadowsViewConfig);
	}

	m_sceneMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
	m_sceneMaterialInstance->GetSlot("global")->SetUniformBuffers("previousCamera", _previousCameraUbos);
	m_sceneMaterialInstance->GetSlot("global")->SetUniformBuffer("res", m_resolutionBuffer.GetValue());

	m_sceneMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLight", _dirLightUbos);
	m_sceneMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLightShadowMat", _shadowsMatricesUbos);
	m_sceneMaterialInstance->GetSlot("global")->SetGpuImages("dirLightShadowMap", _shadowsMaps);

	m_sceneMaterialInstance->GetSlot("global")->FlushUpdate(); // No está completo.
}

void RenderSystem3D::InitializeTerrain(const Vector2ui& resolution, const Texture& heightMap, const Texture& texture) {
/*
	terrain.Generate(resolution);

	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_MipLevelRanged(0, 0);
	terrain.GetMaterialInstance()->GetSlot("global")->SetGpuImage("heightmap", heightMap.GetGpuImage()->GetView(viewConfig));

	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> ubos{};
	for (UIndex32 i = 0; i < 3; i++) ubos[i] = cameraBuffers[i].GetPointer();
	terrain.GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("camera", ubos);

	for (UIndex32 i = 0; i < 3; i++) ubos[i] = dirLightUbos[i].GetPointer();
	terrain.GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("dirLight", ubos);

	for (UIndex32 i = 0; i < 3; i++) ubos[i] = shadowMap.GetDirLightMatrixUniformBuffers()[i];
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
*/
}

void RenderSystem3D::CreateTargetImage(const Vector2ui& size) {
	const RenderTargetAttachmentInfo colorInfo { 
		.format = Format::RGBA16_SFLOAT, 
		.usage = GpuImageUsage::TRANSFER_DESTINATION, 
		.name = "RenderSystem3D Target" 
	};

	const RenderTargetAttachmentInfo motionInfo { 
		.format = Format::RG16_SFLOAT,
		.usage = GpuImageUsage::COLOR | GpuImageUsage::SAMPLED,
		.name = "RenderSystem3D Motion"
	};

	const RenderTargetAttachmentInfo normalInfo{
		.format = Format::RGB10A2_UNORM,
		.usage = GpuImageUsage::COLOR | GpuImageUsage::SAMPLED,
		.name = "RenderSystem3D Normal"
	};

	const RenderTargetAttachmentInfo depthInfo { 
		.format = Format::D32_SFLOAT, 
		.usage = GpuImageUsage::DEPTH | GpuImageUsage::SAMPLED,
		.name = "RenderSystem3D Depth"
	};

	m_renderTarget.Create(size, { colorInfo, motionInfo, normalInfo }, depthInfo);

	// TAA
	std::array<const GpuImage*, NUM_RESOURCES_IN_FLIGHT> sourceImages{};
	std::array<const GpuImage*, NUM_RESOURCES_IN_FLIGHT> motionImages{};

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		sourceImages[i] = m_renderTarget.GetColorImage(COLOR_IMAGE_INDEX, i);
		motionImages[i] = m_renderTarget.GetColorImage(MOTION_IMAGE_INDEX, i);
	}

	m_taaProvider.InitializeTaa(
		size,
		sourceImages,
		motionImages);

	m_resolutionBuffer->MapMemory();
	m_resolutionBuffer->Write(size.ToVector2f());
	m_resolutionBuffer->Unmap();
}

void RenderSystem3D::Resize(const Vector2ui& size) {
	IRenderSystem::Resize(size);

	// TAA
	std::array<const GpuImage*, NUM_RESOURCES_IN_FLIGHT> sourceImages{};
	std::array<const GpuImage*, NUM_RESOURCES_IN_FLIGHT> motionImages{};

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		sourceImages[i] = m_renderTarget.GetColorImage(COLOR_IMAGE_INDEX, i);
		motionImages[i] = m_renderTarget.GetColorImage(MOTION_IMAGE_INDEX, i);
	}

	m_taaProvider.ResizeTaa(
		size,
		sourceImages,
		motionImages);

	m_resolutionBuffer->MapMemory();
	m_resolutionBuffer->Write(size.ToVector2f());
	m_resolutionBuffer->Unmap();
}

ShadowMap* RenderSystem3D::GetShadowMap() {
	return &m_shadowMap;
}

void RenderSystem3D::GenerateShadows(ICommandList* commandList, ModelType modelType) {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	const Viewport viewport {
		.rectangle = { 0u, 0u, 
		m_shadowMap.GetColorImage(0)->GetSize2D().x, 
		m_shadowMap.GetColorImage(0)->GetSize2D().y }
	};

	commandList->StartDebugSection("Shadows", Color::Black);

	commandList->SetGpuImageBarrier(
		m_shadowMap.GetShadowImage(resourceIndex), 
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::DEPTH_STENCIL_TARGET,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::DEPTH_STENCIL_START, GpuAccessStage::DEPTH_STENCIL_READ | GpuAccessStage::DEPTH_STENCIL_WRITE),
		GpuImageRange { 
			.baseLayer = 0, 
			.numLayers = m_shadowMap.GetNumCascades(),
			.baseMipLevel = 0, 
			.numMipLevels = ALL_MIP_LEVELS, 
			.channel = SampledChannel::DEPTH 
		});

	commandList->SetViewport(viewport);
	commandList->SetScissor(viewport.rectangle);

	for (UIndex32 i = 0; i < m_shadowMap.GetNumCascades(); i++) {

		commandList->StartDebugSection(std::format("Cascade {}", i), Color::Black);

		RenderPassImageInfo colorInfo{};
		colorInfo.arrayLevel = i;
		colorInfo.targetImage = m_shadowMap.GetColorImage(resourceIndex);

		RenderPassImageInfo depthInfo{};
		depthInfo.arrayLevel = i;
		depthInfo.targetImage = m_shadowMap.GetShadowImage(resourceIndex);

		commandList->BeginGraphicsRenderpass({ colorInfo }, depthInfo, { 1.0f, 1.0f, 1.0f, 1.0f }, false);

		commandList->StartDebugSection("Static Meshes", Color::Black);

		commandList->BindMaterial(*m_shadowMap.GetShadowsMaterial(ModelType::STATIC_MESH));
		commandList->BindMaterialSlot(*m_shadowMap.GetShadowsMaterialInstance()->GetSlot("global"));

		ShadowsRenderLoop(ModelType::STATIC_MESH, commandList, i);

		commandList->EndDebugSection();

		commandList->StartDebugSection("Animated Models", Color::Black);

		commandList->BindMaterial(*m_shadowMap.GetShadowsMaterial(ModelType::ANIMATED_MODEL));
		commandList->BindMaterialSlot(*m_shadowMap.GetShadowsMaterialInstance()->GetSlot("global"));
		ShadowsRenderLoop(ModelType::ANIMATED_MODEL, commandList, i);

		commandList->EndDebugSection();

		commandList->EndGraphicsRenderpass(false);

		commandList->EndDebugSection();
	}

	commandList->EndDebugSection();
}

void RenderSystem3D::RenderScene(ICommandList* commandList) {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	
	commandList->StartDebugSection("PBR Direct", Color::Red);

	commandList->SetGpuImageBarrier(m_shadowMap.GetShadowImage(resourceIndex),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SAMPLED_READ),
		GpuImageRange{
			.baseLayer = 0, 
			.numLayers = m_shadowMap.GetNumCascades(), 
			.baseMipLevel = 0, 
			.numMipLevels = ALL_MIP_LEVELS, 
			.channel = SampledChannel::DEPTH });

	commandList->BeginGraphicsRenderpass(&m_renderTarget, Color::Black * 0.0f);
	SetupViewport(commandList);

	commandList->StartDebugSection("Static Meshes", Color::Red);
	SceneRenderLoop(ModelType::STATIC_MESH, commandList);
	commandList->EndDebugSection();

	commandList->StartDebugSection("Animated Models", Color::Red);
	// SceneRenderLoop(ModelType::ANIMATED_MODEL, commandList);
	commandList->EndDebugSection();

	if (m_terrain.GetVertexBuffer() != nullptr)
		RenderTerrain(commandList);
	
	commandList->EndGraphicsRenderpass();
	commandList->EndDebugSection();
}

void RenderSystem3D::SceneRenderLoop(ModelType modelType, ICommandList* commandList) {
	const GpuBuffer* previousVertexBuffer = nullptr;
	const GpuBuffer* previousIndexBuffer = nullptr;

	commandList->BindMaterial(modelType == ModelType::STATIC_MESH ? *m_sceneMaterial : *m_animatedSceneMaterial);
	commandList->BindMaterialSlot(*m_sceneMaterialInstance->GetSlot("global"));
	
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
		} pushConsts {
			.model = transform.GetAsMatrix(),
			.previousModel = m_previousModelMatrices.contains(obj) 
				? m_previousModelMatrices.at(obj) 
				: glm::mat4(1.0f)
		};

		if (modelType == ModelType::ANIMATED_MODEL)
			commandList->BindMaterialSlot(*model.GetModel()->GetAnimator()->GetMaterialInstance()->GetSlot("animation"));

		for (UIndex32 meshIndex = 0; meshIndex < model.GetModel()->GetMeshes().GetSize(); meshIndex++) {
			// commandList->BindMaterialSlot(*model.GetMeshMaterialInstance(meshIndex)->GetSlot("texture"));

			const auto& mesh = model.GetModel()->GetMeshes()[meshIndex];
			const auto& meshMetadata = model.GetModel()->GetMetadata().meshesMetadata[meshIndex];

			pushConsts.materialInfos.x = meshMetadata.metallicFactor;
			pushConsts.materialInfos.y = meshMetadata.roughnessFactor;
			pushConsts.materialInfos.z = (float)m_taaProvider.GetCurrentFrameJitterIndex();

			commandList->PushMaterialConstants("pushConstants", pushConsts);

			commandList->DrawSingleMesh(mesh.GetFirstIndexId(), mesh.GetNumberOfIndices());
		}
	}
}

void RenderSystem3D::ShadowsRenderLoop(ModelType modelType, ICommandList* commandList, UIndex32 cascadeIndex) {
	for (const GameObjectIndex obj : GetObjects()) {
		const auto& model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);
		const auto& transform = Engine::GetEcs()->GetComponent<Transform3D>(obj);

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

		for (const auto& mesh : model.GetModel()->GetMeshes())
			commandList->DrawSingleMesh(mesh.GetFirstIndexId(), mesh.GetNumberOfIndices());
	}
}

void RenderSystem3D::RenderTerrain(ICommandList* commandList) {
	commandList->BindMaterial(*m_terrainMaterial);
	commandList->BindMaterialInstance(*m_terrain.GetMaterialInstance());

	commandList->BindVertexBuffer(*m_terrain.GetVertexBuffer());
	commandList->BindIndexBuffer(*m_terrain.GetIndexBuffer());
	
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
	
	commandList->DrawSingleMesh(0, m_terrain.GetNumIndices());
}

void RenderSystem3D::ExecuteTaa(ICommandList* commandList) {
	commandList->StartDebugSection("TAA", Color::Yellow);

	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	commandList->SetGpuImageBarrier(
		m_renderTarget.GetColorImage(MOTION_IMAGE_INDEX, resourceIndex),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	commandList->SetGpuImageBarrier(
		m_renderTarget.GetColorImage(COLOR_IMAGE_INDEX, resourceIndex),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	m_taaProvider.ExecuteTaa(commandList);

	if (m_taaProvider.IsActive())
		CopyTaaResult(commandList);
	
	commandList->EndDebugSection();
}

void RenderSystem3D::CopyTaaResult(GRAPHICS::ICommandList* commandList) {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	GpuImage* sourceImage = m_taaProvider.GetTaaOutput().GetTargetImage(resourceIndex);
	GpuImage* destinationImage = m_renderTarget.GetColorImage(COLOR_IMAGE_INDEX, resourceIndex);

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
		CopyImageInfo::CreateDefault2D(m_renderTarget.GetSize()));
}

void RenderSystem3D::Render(GRAPHICS::ICommandList* commandList) {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	const auto& camera = Engine::GetEcs()->GetComponent<CameraComponent3D>(m_cameraObject);
	const auto& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(m_cameraObject);

	const glm::mat4 currentProjection = camera.GetProjectionMatrix();
	const glm::mat4 currentView = camera.GetViewMatrix(cameraTransform);

	m_cameraBuffers[resourceIndex]->MapMemory();
	m_cameraBuffers[resourceIndex]->Write(currentProjection);
	m_cameraBuffers[resourceIndex]->Write(currentView);
	m_cameraBuffers[resourceIndex]->Write(cameraTransform.GetPosition());
	m_cameraBuffers[resourceIndex]->Unmap();

	m_previousCameraBuffers[resourceIndex]->MapMemory();
	m_previousCameraBuffers[resourceIndex]->Write(m_previousCameraProjection);
	m_previousCameraBuffers[resourceIndex]->Write(m_previousCameraView);
	m_previousCameraBuffers[resourceIndex]->Unmap();

	m_dirLightUbos[resourceIndex]->MapMemory();
	m_dirLightUbos[resourceIndex]->Write(m_dirLight);
	m_dirLightUbos[resourceIndex]->Unmap();

	m_previousCameraProjection = currentProjection;
	m_previousCameraView = currentView;

	m_shadowMap.SetDirectionalLight(m_dirLight);
	
	GenerateShadows(commandList, ModelType::STATIC_MESH);
	RenderScene(commandList);
	ExecuteTaa(commandList);

	for (const GameObjectIndex obj : GetObjects())
		m_previousModelMatrices[obj] = Engine::GetEcs()->GetComponent<Transform3D>(obj).GetAsMatrix();
}

void RenderSystem3D::OnTick(TDeltaTime deltaTime) {
	for (const GameObjectIndex obj : GetObjects()) {
		Model3D* model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj).GetModel();

		if (model->GetType() == ModelType::STATIC_MESH)
			continue;

		model->GetAnimator()->Update(deltaTime);
	}
}
