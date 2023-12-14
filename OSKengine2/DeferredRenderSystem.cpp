#include "DeferredRenderSystem.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"
#include "Transform3D.h"
#include "ModelComponent3D.h"
#include "ICommandList.h"
#include "IRenderer.h"
#include "GpuImageUsage.h"
#include "GpuImageDimensions.h"
#include "GpuMemoryTypes.h"
#include "Format.h"
#include "MaterialInstance.h"
#include "Material.h"
#include "MaterialLayout.h"
#include "Model3D.h"
#include "Mesh3D.h"
#include "Viewport.h"
#include "Window.h"
#include "GpuImageLayout.h"

#include "CameraComponent3D.h"
#include "IrradianceMap.h"
#include "PushConst2D.h"
#include "SpriteRenderer.h"
#include "SpecularMap.h"

#include "MatrixOperations.hpp"

#include "StaticGBufferPass.h"
#include "AnimatedGBufferPass.h"
#include "BillboardGBufferPass.h"
#include "TreeGBufferPass.h"

#include "PbrResolvePass.h"

#include "InvalidObjectStateException.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


DeferredRenderSystem::DeferredRenderSystem() {
	// Signature del sistema
	Signature signature{};
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<ModelComponent3D>());
	_SetSignature(signature);


	// Mapa de sombras
	m_shadowMap.Create(Vector2ui(4096u));

	// Directional light por defecto
	const Vector3f direction = Vector3f(1.0f, -1.9f, 0.0f).GetNormalized();
	m_directionalLight.directionAndIntensity = Vector4f(direction.x, direction.y, direction.z, 1.0f);
	m_directionalLight.color = Color(255 / 255.f, 255 / 255.f, 255 / 255.f);
}


void DeferredRenderSystem::CreateBuffers() {
	IGpuMemoryAllocator* memAllocator = Engine::GetRenderer()->GetAllocator();

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		m_cameraBuffers[i] = memAllocator->CreateUniformBuffer(sizeof(CameraInfo)).GetPointer();
		m_previousFrameCameraBuffers[i] = memAllocator->CreateUniformBuffer(sizeof(PreviousCameraInfo)).GetPointer();
		m_directionalLightBuffers[i] = memAllocator->CreateUniformBuffer(sizeof(DirectionalLight)).GetPointer();
		m_iblConfigBuffers[i] = memAllocator->CreateUniformBuffer(sizeof(PbrIblConfig)).GetPointer();
	}
}

void DeferredRenderSystem::LoadMaterials() {
	MaterialSystem* materialSystem = Engine::GetRenderer()->GetMaterialSystem();

	// Material para el slot de la cámara.
	Material* gBufferMaterial = materialSystem->LoadMaterial("Resources/Materials/PBR/Deferred/deferred_gbuffer_static.json");
	m_gBufferCameraInstance = gBufferMaterial->CreateInstance().GetPointer();
}

void DeferredRenderSystem::SetupGBufferMaterial() {
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> _cameraUbos{};
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> _previousCameraUbos{};

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		_cameraUbos[i] = m_cameraBuffers[i].GetPointer();
		_previousCameraUbos[i] = m_previousFrameCameraBuffers[i].GetPointer();
	}

	m_gBufferCameraInstance->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
	m_gBufferCameraInstance->GetSlot("global")->SetUniformBuffers("previousCamera", _previousCameraUbos);
	m_gBufferCameraInstance->GetSlot("global")->FlushUpdate();
}

void DeferredRenderSystem::Initialize(GameObjectIndex camera, ASSETS::AssetRef<ASSETS::IrradianceMap> irradianceMap, ASSETS::AssetRef<ASSETS::SpecularMap> specularMap) {
	m_cameraObject = camera;


	// Pases por defecto
	AddRenderPass(new StaticGBufferPass());
	AddRenderPass(new AnimatedGBufferPass());
	AddRenderPass(new BillboardGBufferPass());
	AddRenderPass(new TreeGBufferPass());

	SetResolver(new PbrResolverPass());

	CreateBuffers();
	LoadMaterials();
	SetupGBufferMaterial();
	SetupResolveMaterial();

	m_taaProvider.SetMaxJitter(4);
		
	SetIbl(irradianceMap, specularMap);

	m_shadowMap.SetSceneCamera(camera);

	for (auto& pass : m_renderPasses) {
		pass->Load();
		pass->SetCamera(camera);
	}
}

void DeferredRenderSystem::SetIbl(AssetRef<IrradianceMap> irradianceMap, AssetRef<SpecularMap> specularMap) {
	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_Default();
	const GpuImageViewConfig cubemapConfig = GpuImageViewConfig::CreateSampled_Cubemap();

	m_irradianceMap = irradianceMap;
	m_specularMap = specularMap;

	m_resolverPass->GetMaterialInstance()->GetSlot("global")->SetGpuImage("irradianceMap", irradianceMap->GetGpuImage()->GetView(cubemapConfig));
	m_resolverPass->GetMaterialInstance()->GetSlot("global")->SetGpuImage("specularMap", specularMap->GetCubemapImage()->GetView(cubemapConfig));
	m_resolverPass->GetMaterialInstance()->GetSlot("global")->SetGpuImage("specularLut", specularMap->GetLookUpTable()->GetView(viewConfig));
	m_resolverPass->GetMaterialInstance()->GetSlot("global")->FlushUpdate();
}

void DeferredRenderSystem::AddRenderPass(OwnedPtr<GRAPHICS::IRenderPass> pass) {
	IRenderSystem::AddRenderPass(pass);

	pass->SetCamera(m_cameraObject);
	pass->Load();
}

void DeferredRenderSystem::SetResolver(OwnedPtr<GRAPHICS::IDeferredResolver> resolver) {
	m_resolverPass = resolver.GetPointer();
	m_resolverPass->Load();
}

void DeferredRenderSystem::SetupResolveMaterial() {
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> _cameraUbos{};
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> _dirLightUbos{};
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> _shadowsMatricesUbos{};
	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> _shadowsMaps{};
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> _iblConfigs{};

	GpuImageViewConfig shadowsViewConfig = GpuImageViewConfig::CreateSampled_Array(0, m_shadowMap.GetNumCascades());
	shadowsViewConfig.channel = SampledChannel::DEPTH;

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		_cameraUbos[i] = m_cameraBuffers[i].GetPointer();
		_dirLightUbos[i] = m_directionalLightBuffers[i].GetPointer();
		_shadowsMatricesUbos[i] = m_shadowMap.GetDirLightMatrixUniformBuffers()[i];
		_shadowsMaps[i] = m_shadowMap.GetShadowImage(i)->GetView(shadowsViewConfig);
		_iblConfigs[i] = m_iblConfigBuffers[i].GetPointer();
	}

	m_resolverPass->GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
	m_resolverPass->GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("dirLight", _dirLightUbos);
	m_resolverPass->GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("dirLightShadowMat", _shadowsMatricesUbos);
	m_resolverPass->GetMaterialInstance()->GetSlot("global")->SetGpuImages("dirLightShadowMap", _shadowsMaps);
	m_resolverPass->GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("iblConfig", _iblConfigs);
	m_resolverPass->GetMaterialInstance()->GetSlot("global")->FlushUpdate();

	UpdateResolveMaterial();
}

void DeferredRenderSystem::UpdateResolveMaterial() {
	if (!m_resolverPass.GetPointer())
		return;

	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_MipLevelRanged(0, 0);
	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> images{};

	GpuImageViewConfig depthView = viewConfig;
	depthView.channel = SampledChannel::DEPTH;

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) images[i] = m_gBuffer.GetImage(i, GBuffer::Target::DEPTH)->GetView(depthView);
	m_resolverPass->GetMaterialInstance()->GetSlot("gbuffer")->SetGpuImages("depthTexture", images);

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) images[i] = m_gBuffer.GetImage(i, GBuffer::Target::COLOR)->GetView(viewConfig);
	m_resolverPass->GetMaterialInstance()->GetSlot("gbuffer")->SetGpuImages("colorTexture", images);

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) images[i] = m_gBuffer.GetImage(i, GBuffer::Target::NORMAL)->GetView(viewConfig);
	m_resolverPass->GetMaterialInstance()->GetSlot("gbuffer")->SetGpuImages("normalTexture", images);

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) images[i] = m_gBuffer.GetImage(i, GBuffer::Target::METALLIC_ROUGHNESS)->GetView(viewConfig);
	m_resolverPass->GetMaterialInstance()->GetSlot("gbuffer")->SetGpuImages("metallicRoughnessTexture", images);

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) images[i] = m_gBuffer.GetImage(i, GBuffer::Target::EMISSIVE)->GetView(viewConfig);
	m_resolverPass->GetMaterialInstance()->GetSlot("gbuffer")->SetGpuImages("emissiveTexture", images);

	m_resolverPass->GetMaterialInstance()->GetSlot("gbuffer")->FlushUpdate();

	GpuImageViewConfig computeOutputConfig = GpuImageViewConfig::CreateStorage_Default();

	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> _resolveImages{};
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) _resolveImages[i] = m_resolveRenderTarget.GetTargetImage(i)->GetView(computeOutputConfig);

	m_resolverPass->GetMaterialInstance()->GetSlot("output")->SetStorageImages("finalImage", _resolveImages);
	m_resolverPass->GetMaterialInstance()->GetSlot("output")->FlushUpdate();
}

void DeferredRenderSystem::CreateTargetImage(const Vector2ui& size) {
	// GBuffer.
	GpuImageSamplerDesc sampler = GpuImageSamplerDesc::CreateDefault();
	sampler.addressMode = GpuImageAddressMode::EDGE;
	m_gBuffer.Create(size);

	// Resolve
	RenderTargetAttachmentInfo resolveColorAttachment{};
	resolveColorAttachment.format = Format::RGBA16_SFLOAT;
	resolveColorAttachment.usage = GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::COMPUTE;
	resolveColorAttachment.name = "Resolve Target";

	m_resolveRenderTarget.Create(size, resolveColorAttachment);

	// Imagen final.
	RenderTargetAttachmentInfo colorAttachment{};
	colorAttachment.format = Format::RGBA16_SFLOAT;
	colorAttachment.usage = GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::COMPUTE;
	colorAttachment.name = "Deferred Target";

	RenderTargetAttachmentInfo depthAttachment{};
	depthAttachment.format = Format::D32_SFLOAT;
	depthAttachment.usage = GpuImageUsage::DEPTH | GpuImageUsage::SAMPLED;
	depthAttachment.name = "Deferred Target Depth";

	m_renderTarget.Create(size, { colorAttachment }, depthAttachment);

	// TAA
	std::array<const GpuImage*, NUM_RESOURCES_IN_FLIGHT> sourceImages{};
	std::array<const GpuImage*, NUM_RESOURCES_IN_FLIGHT> motionImages{};

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		sourceImages[i] = m_resolveRenderTarget.GetTargetImage(i);
		motionImages[i] = m_gBuffer.GetImage(i, GBuffer::Target::MOTION);
	}

	m_taaProvider.InitializeTaa(
		size,
		sourceImages,
		motionImages);

	UpdateResolveMaterial();
}

void DeferredRenderSystem::Resize(const Vector2ui& windowSize) {
	m_gBuffer.Resize(windowSize);
	m_resolveRenderTarget.Resize(windowSize);

	// TAA
	std::array<const GpuImage*, NUM_RESOURCES_IN_FLIGHT> sourceImages{};
	std::array<const GpuImage*, NUM_RESOURCES_IN_FLIGHT> motionImages{};

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		sourceImages[i] = m_resolveRenderTarget.GetTargetImage(i);
		motionImages[i] = m_gBuffer.GetImage(i, GBuffer::Target::MOTION);
	}

	m_taaProvider.ResizeTaa(
		windowSize,
		sourceImages,
		motionImages);

	IRenderSystem::Resize(windowSize);

	UpdateResolveMaterial();
}

const GBuffer& DeferredRenderSystem::GetGbuffer() const {
	return m_gBuffer;
}

GBuffer& DeferredRenderSystem::GetGbuffer() {
	return m_gBuffer;
}

void DeferredRenderSystem::Render(ICommandList* commandList) {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	const CameraComponent3D& camera = Engine::GetEcs()->GetComponent<CameraComponent3D>(m_cameraObject);
	const Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(m_cameraObject);

	CameraInfo currentCameraInfo{};
	currentCameraInfo.projectionMatrix = camera.GetProjectionMatrix();
	currentCameraInfo.viewMatrix = camera.GetViewMatrix(cameraTransform);
	currentCameraInfo.projectionViewMatrix = currentCameraInfo.projectionMatrix * currentCameraInfo.viewMatrix;
	currentCameraInfo.position = glm::vec4(cameraTransform.GetPosition().ToGlm(), 1.0f);
	currentCameraInfo.nearFarPlanes = { camera.GetNearPlane(), camera.GetFarPlane() };

	PreviousCameraInfo previousCameraInfo{};
	previousCameraInfo.projectionMatrix = m_previousCameraProjection;
	previousCameraInfo.viewMatrix = m_previousCameraView;
	previousCameraInfo.projectionViewMatrix = m_previousCameraProjection * m_previousCameraView;

	m_cameraBuffers[resourceIndex]->MapMemory();
	m_cameraBuffers[resourceIndex]->Write(currentCameraInfo);
	m_cameraBuffers[resourceIndex]->Unmap();

	m_previousFrameCameraBuffers[resourceIndex]->MapMemory();
	m_previousFrameCameraBuffers[resourceIndex]->Write(previousCameraInfo);
	m_previousFrameCameraBuffers[resourceIndex]->Unmap();

	m_previousCameraProjection = camera.GetProjectionMatrix();
	m_previousCameraView = camera.GetViewMatrix(cameraTransform);

	m_directionalLightBuffers[resourceIndex]->MapMemory();
	m_directionalLightBuffers[resourceIndex]->Write(m_directionalLight);
	m_directionalLightBuffers[resourceIndex]->Unmap();

	m_iblConfigBuffers[resourceIndex]->MapMemory();
	m_iblConfigBuffers[resourceIndex]->Write(m_iblConfig);
	m_iblConfigBuffers[resourceIndex]->Unmap();

	m_shadowMap.SetDirectionalLight(m_directionalLight);
	
	GenerateShadows(commandList);

	commandList->StartDebugSection("PBR Deferred", Color::Red);
	
	UpdatePerPassObjectLists();

	RenderGBuffer(commandList);
	ResolveGBuffer(commandList);

	commandList->EndDebugSection();

	ExecuteTaa(commandList);
	CopyFinalImages(commandList);
}

void DeferredRenderSystem::GenerateShadows(ICommandList* commandList) {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	const Viewport viewport{
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
		GpuImageRange{
			.baseLayer = 0, 
			.numLayers = m_shadowMap.GetNumCascades(), 
			.baseMipLevel = 0, 
			.numMipLevels = ALL_MIP_LEVELS, 
			.channel = SampledChannel::DEPTH });

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

		commandList->BeginGraphicsRenderpass({ colorInfo }, depthInfo, { 1.0f, 1.0f, 1.0f, 1.0f });

		commandList->BindMaterial(*m_shadowMap.GetShadowsMaterial(ASSETS::ModelType::STATIC_MESH));
		commandList->BindMaterialSlot(*m_shadowMap.GetShadowsMaterialInstance()->GetSlot("global"));

		ShadowsRenderLoop(ModelType::STATIC_MESH, commandList, i);
		
		commandList->BindMaterial(*m_shadowMap.GetShadowsMaterial(ModelType::ANIMATED_MODEL));
		commandList->BindMaterialSlot(*m_shadowMap.GetShadowsMaterialInstance()->GetSlot("global"));

		ShadowsRenderLoop(ModelType::ANIMATED_MODEL, commandList, i);

		commandList->EndGraphicsRenderpass();

		commandList->EndDebugSection();
	}

	commandList->EndDebugSection();
}

void DeferredRenderSystem::ShadowsRenderLoop(ModelType modelType, ICommandList* commandList, UIndex32 cascadeIndex) {
	for (const GameObjectIndex obj : GetObjects()) {
		const ModelComponent3D& model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);
		const Transform3D& transform = Engine::GetEcs()->GetComponent<Transform3D>(obj);

		if (model.GetModel()->GetType() != modelType)
			continue;

		if (!model.CastsShadows())
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

		for (UIndex32 i = 0; i < model.GetModel()->GetMeshes().GetSize(); i++)
			commandList->DrawSingleMesh(
				model.GetModel()->GetMeshes()[i].GetFirstIndexId(), 
				model.GetModel()->GetMeshes()[i].GetNumberOfIndices());
	}
}

void DeferredRenderSystem::RenderGBuffer(ICommandList* commandList) {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	commandList->StartDebugSection("PBR GBuffer", Color::Red);

	m_gBuffer.BeginRenderpass(commandList, Color::Black * 0.0f);

	SetupViewport(commandList);

	// Bindeamos el slot de la cámara.
	commandList->BindMaterial(*m_gBufferCameraInstance->GetMaterial());

	static UIndex32 jitterIndex = 0;
	constexpr UIndex32 maxJitter = 4;

	jitterIndex = m_taaProvider.GetCurrentFrameJitterIndex();

	const Vector2ui resolution = m_gBuffer.GetImage(resourceIndex, GBuffer::Target::COLOR)->GetSize2D();

	for (auto& pass : m_renderPasses) {
		OSK_ASSERT(
			m_objectsPerPass.contains(pass->GetTypeName()), 
			InvalidObjectStateException(std::format("No se encuentra el pase {}", pass->GetTypeName())));

		commandList->BindMaterialSlot(*m_gBufferCameraInstance->GetSlot("global"));
		const auto& objectList = m_objectsPerPass.find(pass->GetTypeName())->second;
		pass->RenderLoop(commandList, objectList, jitterIndex, resolution);
	}

	commandList->EndGraphicsRenderpass();

	commandList->EndDebugSection();
}

void DeferredRenderSystem::ResolveGBuffer(ICommandList* commandList) {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	commandList->StartDebugSection("PBR Resolve", Color::Purple);

	// Sincronización de mapa de sombras.
	commandList->SetGpuImageBarrier(
		m_shadowMap.GetShadowImage(resourceIndex),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ),
		GpuImageRange{
			.baseLayer = 0,
			.numLayers = m_shadowMap.GetNumCascades(),
			.baseMipLevel = 0,
			.numMipLevels = ALL_MIP_LEVELS,
			.channel = SampledChannel::DEPTH });

	// Sincronización de render target.
	commandList->SetGpuImageBarrier(
		m_resolveRenderTarget.GetTargetImage(resourceIndex),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));


	// Sincronización con todos los targets de color.
	for (const auto type : GBuffer::ColorTargetTypes)
		commandList->SetGpuImageBarrier(
			m_gBuffer.GetImage(resourceIndex, type),
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	// Sincronización con depth.
	commandList->SetGpuImageBarrier(
		m_gBuffer.GetImage(resourceIndex, GBuffer::Target::DEPTH),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ),
		{ .channel = SampledChannel::DEPTH });

	m_resolverPass->RenderLoop(
		commandList, {}, 
		m_taaProvider.GetCurrentFrameJitterIndex(), 
		m_resolveRenderTarget.GetSize());

	commandList->EndDebugSection();
}

void DeferredRenderSystem::ExecuteTaa(ICommandList* commandList) {
	commandList->StartDebugSection("TAA", Color::Yellow);

	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	GpuImage* sourceImage = m_resolveRenderTarget.GetTargetImage(resourceIndex);

	commandList->SetGpuImageBarrier(
		sourceImage,
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ));

	m_taaProvider.ExecuteTaa(commandList);

	commandList->SetGpuImageBarrier(
		m_taaProvider.GetTaaOutput().GetTargetImage(resourceIndex),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	commandList->EndDebugSection();
}

void DeferredRenderSystem::CopyFinalImages(ICommandList* cmdList) {
	cmdList->StartDebugSection("Final Image Copy", Color::Blue);

	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	GpuImage* sourceImage = m_taaProvider.IsActive()
		? m_taaProvider.GetTaaOutput().GetTargetImage(resourceIndex)
		: m_resolveRenderTarget.GetTargetImage(resourceIndex);
	GpuImage* targetImage = m_renderTarget.GetMainColorImage(resourceIndex);


	// Imagen original: transfer source.
	cmdList->SetGpuImageBarrier(
		sourceImage,
		GpuImageLayout::TRANSFER_SOURCE,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_READ));
	
	// Imagen del render target final: transfer destination.
	cmdList->SetGpuImageBarrier(
		targetImage,
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE));

	cmdList->RawCopyImageToImage(
		*sourceImage,
		targetImage,
		CopyImageInfo::CreateDefault2D(m_resolveRenderTarget.GetSize()));

	cmdList->EndDebugSection();
}

void DeferredRenderSystem::OnTick(TDeltaTime deltaTime) {
	for (const GameObjectIndex obj : GetObjects()) {
		Model3D* model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj).GetModel();
		Transform3D transformCopy = Engine::GetEcs()->GetComponent<Transform3D>(obj);
		transformCopy.SetScale(Vector3f::One);

		for (UIndex32 i = 0; i < model->GetMeshes().GetSize(); i++) {
			auto& mesh = model->GetMeshes()[i];
			const Vector3f spherePosition = Math::TransformPoint(mesh.GetSphereCenter(), transformCopy.GetAsMatrix());
			mesh.GetBounds().SetPosition(spherePosition);
		}

		if (model->GetType() == ModelType::STATIC_MESH)
			continue;

		model->GetAnimator()->Update(deltaTime);
	}
}

void DeferredRenderSystem::ToggleTaa() {
	m_taaProvider.ToggleActivation();
}

DirectionalLight& DeferredRenderSystem::GetDirectionalLight() {
	return m_directionalLight;
}

const DirectionalLight& DeferredRenderSystem::GetDirectionalLight() const {
	return m_directionalLight;
}

PbrIblConfig& DeferredRenderSystem::GetIblConfig() {
	return m_iblConfig;
}

const PbrIblConfig& DeferredRenderSystem::GetIblConfig() const {
	return m_iblConfig;
}

GRAPHICS::ShadowMap& DeferredRenderSystem::GetShadowMap() {
	return m_shadowMap;
}

const GRAPHICS::ShadowMap& DeferredRenderSystem::GetShadowMap() const {
	return m_shadowMap;
}
