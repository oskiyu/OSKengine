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
#include "ShadowsStaticPass.h"

#include "PbrResolvePass.h"

#include "InvalidObjectStateException.h"
#include "CopyImageInfo.h"


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
	AddShaderPass(new StaticGBufferPass());
	AddShaderPass(new AnimatedGBufferPass());
	AddShaderPass(new BillboardGBufferPass());
	AddShaderPass(new TreeGBufferPass());

	auto* shadowsPass = new ShadowsStaticPass();
	AddShadowsPass(shadowsPass);
	shadowsPass->SetupShadowMap(m_shadowMap);

	SetResolver(new PbrResolverPass());

	CreateBuffers();
	LoadMaterials();
	SetupGBufferMaterial();
	SetupResolveMaterial();

	m_taaProvider.SetMaxJitter(4);
		
	SetIbl(irradianceMap, specularMap);

	m_shadowMap.SetSceneCamera(camera);

	for (auto& pass : m_shaderPasses.GetAllPasses()) {
		pass->Load();
		pass->SetCamera(camera);
	}

	for (auto& pass : m_shadowsPasses.GetAllPasses()) {
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

void DeferredRenderSystem::AddShaderPass(OwnedPtr<GRAPHICS::IShaderPass> pass) {
	IRenderSystem::AddShaderPass(pass);

	pass->SetCamera(m_cameraObject);
	pass->Load();
}

void DeferredRenderSystem::AddShadowsPass(OwnedPtr<GRAPHICS::IShaderPass> pass) {
	IRenderSystem::AddShadowsPass(pass);

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
	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> _iblConfigs{};

	GpuImageViewConfig shadowsViewConfig = GpuImageViewConfig::CreateSampled_Array(0, m_shadowMap.GetNumCascades());
	shadowsViewConfig.channel = SampledChannel::DEPTH;

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		_cameraUbos[i] = m_cameraBuffers[i].GetPointer();
		_dirLightUbos[i] = m_directionalLightBuffers[i].GetPointer();
		_shadowsMatricesUbos[i] = m_shadowMap.GetDirLightMatrixUniformBuffers()[i];
		_iblConfigs[i] = m_iblConfigBuffers[i].GetPointer();
	}

	m_resolverPass->GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
	m_resolverPass->GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("dirLight", _dirLightUbos);
	m_resolverPass->GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("dirLightShadowMat", _shadowsMatricesUbos);
	m_resolverPass->GetMaterialInstance()->GetSlot("global")->SetGpuImage("dirLightShadowMap", m_shadowMap.GetShadowImage()->GetView(shadowsViewConfig));
	m_resolverPass->GetMaterialInstance()->GetSlot("global")->SetUniformBuffers("iblConfig", _iblConfigs);
	m_resolverPass->GetMaterialInstance()->GetSlot("global")->FlushUpdate();

	UpdateResolveMaterial();
}

void DeferredRenderSystem::UpdateResolveMaterial() {
	if (!m_resolverPass.GetPointer())
		return;

	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_MipLevelRanged(0, 0);

	GpuImageViewConfig depthViewConfig = viewConfig;
	depthViewConfig.channel = SampledChannel::DEPTH;

	const auto* depthView = m_gBuffer.GetImage(GBuffer::Target::DEPTH)->GetView(depthViewConfig);
	m_resolverPass->GetMaterialInstance()->GetSlot("gbuffer")->SetGpuImage("depthTexture", depthView);

	const auto* colorView = m_gBuffer.GetImage(GBuffer::Target::COLOR)->GetView(viewConfig);
	m_resolverPass->GetMaterialInstance()->GetSlot("gbuffer")->SetGpuImage("colorTexture", colorView);

	const auto* normalView = m_gBuffer.GetImage(GBuffer::Target::NORMAL)->GetView(viewConfig);
	m_resolverPass->GetMaterialInstance()->GetSlot("gbuffer")->SetGpuImage("normalTexture", normalView);

	const auto* metallicView = m_gBuffer.GetImage(GBuffer::Target::METALLIC_ROUGHNESS)->GetView(viewConfig);
	m_resolverPass->GetMaterialInstance()->GetSlot("gbuffer")->SetGpuImage("metallicRoughnessTexture", metallicView);

	const auto* emissiveView = m_gBuffer.GetImage(GBuffer::Target::EMISSIVE)->GetView(viewConfig);
	m_resolverPass->GetMaterialInstance()->GetSlot("gbuffer")->SetGpuImage("emissiveTexture", emissiveView);

	m_resolverPass->GetMaterialInstance()->GetSlot("gbuffer")->FlushUpdate();

	GpuImageViewConfig computeOutputConfig = GpuImageViewConfig::CreateStorage_Default();

	m_resolverPass->GetMaterialInstance()->GetSlot("output")->SetStorageImage("finalImage", 
		m_resolveRenderTarget.GetTargetImage()->GetView(computeOutputConfig));
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
	m_taaProvider.InitializeTaa(
		size,
		m_resolveRenderTarget.GetTargetImage(),
		m_gBuffer.GetImage(GBuffer::Target::MOTION));

	UpdateResolveMaterial();
}

void DeferredRenderSystem::Resize(const Vector2ui& windowSize) {
	m_gBuffer.Resize(windowSize);
	m_resolveRenderTarget.Resize(windowSize);

	// TAA
	m_taaProvider.InitializeTaa(
		windowSize,
		m_resolveRenderTarget.GetTargetImage(),
		m_gBuffer.GetImage(GBuffer::Target::MOTION));

	IRenderSystem::Resize(windowSize);

	UpdateResolveMaterial();
}

const GBuffer& DeferredRenderSystem::GetGbuffer() const {
	return m_gBuffer;
}

GBuffer& DeferredRenderSystem::GetGbuffer() {
	return m_gBuffer;
}

void DeferredRenderSystem::Render(ICommandList* commandList, std::span<const ECS::GameObjectIndex> objects) {
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
	
	m_gBuffer.BindPipelineBarriers(commandList);

	UpdatePerPassObjectLists(objects);

	// MeshMapping
	for (auto& pass : m_shaderPasses.GetAllPasses()) {
		pass->UpdateLocalMeshMapping(m_shaderPasses.GetCompatibleObjects(pass->GetTypeName()));
	}

	for (auto& pass : m_shadowsPasses.GetAllPasses()) {
		pass->UpdateLocalMeshMapping(m_shadowsPasses.GetCompatibleObjects(pass->GetTypeName()));
	}

	for (const auto& obj : objects) {
		if (!m_meshMapping.HasPreviousModelMatrix(obj)) {
			const auto& transform = Engine::GetEcs()->GetComponent<Transform3D>(obj);
			m_meshMapping.SetPreviousModelMatrix(obj, transform.GetAsMatrix());
		}
	}

	GenerateShadows(commandList);

	commandList->StartDebugSection("PBR Deferred", Color::Red);
	
	RenderGBuffer(commandList);
	ResolveGBuffer(commandList);

	commandList->EndDebugSection();

	ExecuteTaa(commandList);
	CopyFinalImages(commandList);

	for (const auto& obj : objects) {
		const auto& transform = Engine::GetEcs()->GetComponent<Transform3D>(obj);
		m_meshMapping.SetPreviousModelMatrix(obj, transform.GetAsMatrix());
	}
}

void DeferredRenderSystem::GenerateShadows(ICommandList* commandList) {
	const Viewport viewport{
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
		colorInfo.targetImage = m_shadowMap.GetColorImage();

		RenderPassImageInfo depthInfo{};
		depthInfo.arrayLevel = i;
		depthInfo.targetImage = m_shadowMap.GetShadowImage();

		commandList->BeginGraphicsRenderpass({ colorInfo }, depthInfo, { 1.0f, 1.0f, 1.0f, 1.0f }, false);

		for (auto& pass : m_shadowsPasses.GetAllPasses()) {
			pass->As<IShadowsPass>()->ShadowsRenderLoop(
				commandList,
				m_shadowsPasses.GetCompatibleObjects(pass->GetTypeName()),
				i,
				m_shadowMap);
		}

		commandList->EndGraphicsRenderpass();

		commandList->EndDebugSection();
	}

	commandList->EndDebugSection();
}

void DeferredRenderSystem::RenderGBuffer(ICommandList* commandList) {
	commandList->StartDebugSection("PBR GBuffer", Color::Red);

	m_gBuffer.BeginRenderpass(commandList, Color::Black * 0.0f, false);

	SetupViewport(commandList);

	// Bindeamos el slot de la cámara.
	commandList->BindMaterial(*m_gBufferCameraInstance->GetMaterial());

	static UIndex32 jitterIndex = 0;
	constexpr UIndex32 maxJitter = 4;

	jitterIndex = m_taaProvider.GetCurrentFrameJitterIndex();

	const Vector2ui resolution = m_gBuffer.GetImage(GBuffer::Target::COLOR)->GetSize2D();

	for (auto& pass : m_shaderPasses.GetAllPasses()) {
		// OSK_ASSERT(
		//	m_objectsPerPass.contains(pass->GetTypeName()), 
		//	InvalidObjectStateException(std::format("No se encuentra el pase {}", pass->GetTypeName())));
		
		commandList->BindMaterialSlot(*m_gBufferCameraInstance->GetSlot("global"));
		const auto& objectList = m_shaderPasses.GetCompatibleObjects(pass->GetTypeName());
		pass->RenderLoop(commandList, objectList, &m_meshMapping, jitterIndex, resolution);
	}

	commandList->EndGraphicsRenderpass();

	commandList->EndDebugSection();
}

void DeferredRenderSystem::ResolveGBuffer(ICommandList* commandList) {
	commandList->StartDebugSection("PBR Resolve", Color::Purple);

	// Sincronización de mapa de sombras.
	commandList->SetGpuImageBarrier(
		m_shadowMap.GetShadowImage(),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COLOR_ATTACHMENT_OUTPUT, GpuAccessStage::COLOR_ATTACHMENT_WRITE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ),
		GpuImageRange{
			.baseLayer = 0,
			.numLayers = m_shadowMap.GetNumCascades(),
			.baseMipLevel = 0,
			.numMipLevels = ALL_MIP_LEVELS,
			.channel = SampledChannel::DEPTH });

	// Sincronización de render target.
	commandList->SetGpuImageBarrier(
		m_resolveRenderTarget.GetTargetImage(),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));


	// Sincronización con todos los targets de color.
	for (const auto type : GBuffer::ColorTargetTypes)
		commandList->SetGpuImageBarrier(
			m_gBuffer.GetImage(type),
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	// Sincronización con depth.
	commandList->SetGpuImageBarrier(
		m_gBuffer.GetImage(GBuffer::Target::DEPTH),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ),
		{ .channel = SampledChannel::DEPTH });

	m_resolverPass->RenderLoop(
		commandList, {}, &m_meshMapping,
		m_taaProvider.GetCurrentFrameJitterIndex(), 
		m_resolveRenderTarget.GetSize());

	commandList->EndDebugSection();
}

void DeferredRenderSystem::ExecuteTaa(ICommandList* commandList) {
	commandList->StartDebugSection("TAA", Color::Yellow);

	GpuImage* sourceImage = m_resolveRenderTarget.GetTargetImage();

	commandList->SetGpuImageBarrier(
		sourceImage,
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ));

	m_taaProvider.ExecuteTaa(commandList);

	commandList->SetGpuImageBarrier(
		m_taaProvider.GetTaaOutput().GetTargetImage(),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	commandList->EndDebugSection();
}

void DeferredRenderSystem::CopyFinalImages(ICommandList* cmdList) {
	cmdList->StartDebugSection("Final Image Copy", Color::Blue);

	GpuImage* sourceImage = m_taaProvider.IsActive()
		? m_taaProvider.GetTaaOutput().GetTargetImage()
		: m_resolveRenderTarget.GetTargetImage();
	GpuImage* targetImage = m_renderTarget.GetMainColorImage();


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

void DeferredRenderSystem::Execute(TDeltaTime deltaTime, std::span<const ECS::GameObjectIndex> objects) {
	for (const GameObjectIndex obj : objects) {
		auto& model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);
		Transform3D transformCopy = Engine::GetEcs()->GetComponent<Transform3D>(obj);
		transformCopy.SetScale(Vector3f::One);

		for (UIndex32 i = 0; i < model.GetModel()->GetMeshes().GetSize(); i++) {
			auto& mesh = model.GetModel()->GetMeshes()[i];
			const Vector3f spherePosition = Math::TransformPoint(mesh.GetSphereCenter(), transformCopy.GetAsMatrix());
			mesh.GetBounds().SetPosition(spherePosition);
		}

		model.GetAnimator().Update(deltaTime);
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
