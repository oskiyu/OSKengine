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
#include "GpuMesh3D.h"
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
#include "CopyImageInfo.h"

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

	for (auto& mInstance : m_sceneMaterialInstances) {
		mInstance->GetSlot("global")->SetGpuImage("irradianceMap", *irradianceMap.GetGpuImage()->GetView(cubemapView));
		mInstance->GetSlot("global")->SetGpuImage("specularMap", *specularMap.GetCubemapImage()->GetView(cubemapView));
		mInstance->GetSlot("global")->SetGpuImage("specularLut", *specularMap.GetLookUpTable()->GetView(viewConfig));
		mInstance->GetSlot("global")->FlushUpdate();
	}

	// terrain.GetMaterialInstance()->GetSlot("global")->SetGpuImage("irradianceMap", irradianceMap.GetGpuImage()->GetView(cubemapView));
	// terrain.GetMaterialInstance()->GetSlot("global")->SetGpuImage("specularMap", specularMap.GetGpuImage());
	// terrain.GetMaterialInstance()->GetSlot("global")->FlushUpdate();

	m_shadowMap.SetSceneCamera(camera);
}

void RenderSystem3D::CreateBuffers() {
	for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
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
	for (auto& mInstance : m_sceneMaterialInstances) {
		mInstance = m_sceneMaterial->CreateInstance().GetPointer();
	}
	m_animatedSceneMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/animated_direct_pbr.json");
}

void RenderSystem3D::SetupMaterials() {
	GpuImageViewConfig shadowsViewConfig = GpuImageViewConfig::CreateSampled_Array(0, m_shadowMap.GetNumCascades());
	shadowsViewConfig.channel = SampledChannel::DEPTH;

	for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		MaterialInstance* materialInstance = m_sceneMaterialInstances[i].GetPointer();
		IMaterialSlot* materialSlot = materialInstance->GetSlot("global");

		materialSlot->SetUniformBuffer("camera", *m_cameraBuffers[i].GetPointer());
		materialSlot->SetUniformBuffer("previousCamera", *m_previousCameraBuffers[i].GetPointer());
		materialSlot->SetUniformBuffer("res", m_resolutionBuffer.GetValue());

		materialSlot->SetUniformBuffer("dirLight", *m_dirLightUbos[i].GetPointer());
		materialSlot->SetUniformBuffer("dirLightShadowMat", *m_shadowMap.GetDirLightMatrixUniformBuffers()[i]);
		materialSlot->SetGpuImage("dirLightShadowMap", *m_shadowMap.GetShadowImage()->GetView(shadowsViewConfig));

		materialSlot->FlushUpdate(); // No está completo.
	}
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
	const RenderTargetAttachmentInfo colorInfo{
		.format = Format::RGBA16_SFLOAT,
		.usage = GpuImageUsage::TRANSFER_DESTINATION,
		.name = "RenderSystem3D Target"
	};

	const RenderTargetAttachmentInfo motionInfo{
		.format = Format::RG16_SFLOAT,
		.usage = GpuImageUsage::COLOR | GpuImageUsage::SAMPLED,
		.name = "RenderSystem3D Motion"
	};

	const RenderTargetAttachmentInfo normalInfo{
		.format = Format::RGB10A2_UNORM,
		.usage = GpuImageUsage::COLOR | GpuImageUsage::SAMPLED,
		.name = "RenderSystem3D Normal"
	};

	const RenderTargetAttachmentInfo depthInfo{
		.format = Format::D32_SFLOAT,
		.usage = GpuImageUsage::DEPTH | GpuImageUsage::SAMPLED,
		.name = "RenderSystem3D Depth"
	};

	m_renderTarget.Create(size, { colorInfo, motionInfo, normalInfo }, depthInfo);

	// TAA
	m_taaProvider.InitializeTaa(
		size,
		m_renderTarget.GetColorImage(COLOR_IMAGE_INDEX),
		m_renderTarget.GetColorImage(MOTION_IMAGE_INDEX));

	m_resolutionBuffer->MapMemory();
	m_resolutionBuffer->Write(size.ToVector2f());
	m_resolutionBuffer->Unmap();
}

void RenderSystem3D::Resize(const Vector2ui& size) {
	IRenderSystem::Resize(size);

	// TAA
	m_taaProvider.InitializeTaa(
		size,
		m_renderTarget.GetColorImage(COLOR_IMAGE_INDEX),
		m_renderTarget.GetColorImage(MOTION_IMAGE_INDEX));

	m_resolutionBuffer->MapMemory();
	m_resolutionBuffer->Write(size.ToVector2f());
	m_resolutionBuffer->Unmap();
}

ShadowMap* RenderSystem3D::GetShadowMap() {
	return &m_shadowMap;
}

void RenderSystem3D::GenerateShadows(ICommandList* commandList) {
	const Viewport viewport {
		.rectangle = { 0u, 0u, 
		m_shadowMap.GetColorImage()->GetSize2D().x, 
		m_shadowMap.GetColorImage()->GetSize2D().y }
	};

	commandList->StartDebugSection("Shadows", Color::Black);

	commandList->SetGpuImageBarrier(
		m_shadowMap.GetShadowImage(), 
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
		colorInfo.targetImage = m_shadowMap.GetColorImage();

		RenderPassImageInfo depthInfo{};
		depthInfo.arrayLevel = i;
		depthInfo.targetImage = m_shadowMap.GetShadowImage();

		commandList->BeginGraphicsRenderpass({ colorInfo }, depthInfo, { 1.0f, 1.0f, 1.0f, 1.0f }, false);

		commandList->StartDebugSection("Static Meshes", Color::Black);

		for (auto& pass : m_shadowsPasses.GetAllPasses()) {
			pass->RenderLoop(
				commandList,
				m_shadowsPasses.GetCompatibleObjects(pass->GetTypeName()),
				&m_meshMapping,
				0,
				m_shadowMap.GetShadowImage()->GetSize2D());
		}

		commandList->EndDebugSection();

		commandList->EndGraphicsRenderpass(false);

		commandList->EndDebugSection();
	}

	commandList->EndDebugSection();
}

void RenderSystem3D::RenderScene(ICommandList* commandList) {
	commandList->StartDebugSection("PBR Direct", Color::Red);

	commandList->SetGpuImageBarrier(m_shadowMap.GetShadowImage(),
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

	for (auto& pass : m_shaderPasses.GetAllPasses()) {
		pass->RenderLoop(commandList,
			m_shaderPasses.GetCompatibleObjects(pass->GetTypeName()),
			&m_meshMapping,
			m_taaProvider.GetCurrentFrameJitterIndex(),
			m_renderTarget.GetSize());
	}

	if (m_terrain.GetVertexBuffer() != nullptr)
		RenderTerrain(commandList);
	
	commandList->EndGraphicsRenderpass();
	commandList->EndDebugSection();
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

	commandList->SetGpuImageBarrier(
		m_renderTarget.GetColorImage(MOTION_IMAGE_INDEX),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	commandList->SetGpuImageBarrier(
		m_renderTarget.GetColorImage(COLOR_IMAGE_INDEX),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	m_taaProvider.ExecuteTaa(commandList);

	if (m_taaProvider.IsActive())
		CopyTaaResult(commandList);
	
	commandList->EndDebugSection();
}

void RenderSystem3D::CopyTaaResult(GRAPHICS::ICommandList* commandList) {
	GpuImage* sourceImage = m_taaProvider.GetTaaOutput().GetTargetImage();
	GpuImage* destinationImage = m_renderTarget.GetColorImage(COLOR_IMAGE_INDEX);

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

void RenderSystem3D::Render(GRAPHICS::ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) {
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
	
	GenerateShadows(commandList);
	RenderScene(commandList);
	ExecuteTaa(commandList);

	for (const GameObjectIndex obj : objects)
		m_previousModelMatrices[obj] = Engine::GetEcs()->GetComponent<Transform3D>(obj).GetAsMatrix();
}

void RenderSystem3D::Execute(TDeltaTime deltaTime, std::span<const ECS::GameObjectIndex> objects) {
	for (const GameObjectIndex obj : objects) {
		auto* model = &Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);

		model->GetAnimator().Update(deltaTime);
	}
}

nlohmann::json RenderSystem3D::SaveConfiguration() const {
	OSK_ASSERT(false, NotImplementedException());
}

PERSISTENCE::BinaryBlock RenderSystem3D::SaveBinaryConfiguration() const {
	auto output = PERSISTENCE::BinaryBlock::Empty();

	return output;
}

void RenderSystem3D::ApplyConfiguration(const nlohmann::json& config, const SavedGameObjectTranslator& translator) {
	OSK_ASSERT(false, NotImplementedException());
}

void RenderSystem3D::ApplyConfiguration(PERSISTENCE::BinaryBlockReader* reader, const SavedGameObjectTranslator& translator) {
	
}
