#include "PbrDeferredRenderSystem.h"

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

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


PbrDeferredRenderSystem::PbrDeferredRenderSystem() {
	// Signature del sistema
	Signature signature{};
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<ModelComponent3D>());
	_SetSignature(signature);

	// Mapa de sombras
	shadowMap.Create({ 4096u });

	// Directional light por defecto
	const Vector3f direction = Vector3f(1.0f, -1.9f, 0.0f).GetNormalized();
	dirLight.directionAndIntensity = Vector4f(direction.X, direction.Y, direction.Z, 1.4f);
	dirLight.color = Color(255 / 255.f, 255 / 255.f, 255 / 255.f);
	
	CreateBuffers();

	LoadMaterials();
	SetupGBufferMaterial();
	SetupResolveMaterial();
}

void PbrDeferredRenderSystem::CreateBuffers() {
	IGpuMemoryAllocator* memAllocator = Engine::GetRenderer()->GetAllocator();

	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		cameraUbos[i] = memAllocator->CreateUniformBuffer(sizeof(CameraInfo)).GetPointer();
		previousCameraUbos[i] = memAllocator->CreateUniformBuffer(sizeof(PreviousCameraInfo)).GetPointer();
		dirLightUbos[i] = memAllocator->CreateUniformBuffer(sizeof(DirectionalLight)).GetPointer();
	}
}

void PbrDeferredRenderSystem::LoadMaterials() {
	MaterialSystem* materialSystem = Engine::GetRenderer()->GetMaterialSystem();

	// Material del resolve final.
	resolveMaterial = materialSystem->LoadMaterial("Resources/Materials/PBR/Deferred/deferred_resolve.json");
	resolveMaterialInstance = resolveMaterial->CreateInstance().GetPointer();

	// Material del renderizado del gbuffer.
	gbufferMaterial = materialSystem->LoadMaterial("Resources/Materials/PBR/Deferred/deferred_gbuffer.json");
	animatedGbufferMaterial = materialSystem->LoadMaterial("Resources/Materials/PBR/Deferred/deferred_gbuffer_anim.json");
	globalGbufferMaterialInstance = gbufferMaterial->CreateInstance().GetPointer();
}

void PbrDeferredRenderSystem::SetupGBufferMaterial() {
	const GpuBuffer* _cameraUbos[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuBuffer* _previousCameraUbos[NUM_RESOURCES_IN_FLIGHT]{};

	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		_cameraUbos[i] = cameraUbos[i].GetPointer();
		_previousCameraUbos[i] = previousCameraUbos[i].GetPointer();
	}

	globalGbufferMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
	globalGbufferMaterialInstance->GetSlot("global")->SetUniformBuffers("previousCamera", _previousCameraUbos);
	globalGbufferMaterialInstance->GetSlot("global")->FlushUpdate();
}

void PbrDeferredRenderSystem::Initialize(GameObjectIndex camera, const ASSETS::IrradianceMap& irradianceMap, const ASSETS::SpecularMap& specularMap) {
	cameraObject = camera;

	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_Default();
	const GpuImageViewConfig cubemapConfig = GpuImageViewConfig::CreateSampled_Cubemap();

	resolveMaterialInstance->GetSlot("global")->SetGpuImage("irradianceMap", irradianceMap.GetGpuImage()->GetView(cubemapConfig));
	resolveMaterialInstance->GetSlot("global")->SetGpuImage("specularMap", specularMap.GetCubemapImage()->GetView(cubemapConfig));
	resolveMaterialInstance->GetSlot("global")->SetGpuImage("specularLut", specularMap.GetLookUpTable()->GetView(viewConfig));
	resolveMaterialInstance->GetSlot("global")->FlushUpdate();

	shadowMap.SetSceneCamera(camera);

	hasBeenInitialized = true;
}

void PbrDeferredRenderSystem::SetupResolveMaterial() {
	const GpuBuffer* _cameraUbos[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuBuffer* _dirLightUbos[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuBuffer* _shadowsMatricesUbos[NUM_RESOURCES_IN_FLIGHT]{};
	const IGpuImageView* _shadowsMaps[NUM_RESOURCES_IN_FLIGHT]{};

	GpuImageViewConfig shadowsViewConfig = GpuImageViewConfig::CreateSampled_Array(0, shadowMap.GetNumCascades());
	shadowsViewConfig.channel = SampledChannel::DEPTH;

	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		_cameraUbos[i] = cameraUbos[i].GetPointer();
		_dirLightUbos[i] = dirLightUbos[i].GetPointer();
		_shadowsMatricesUbos[i] = shadowMap.GetDirLightMatrixUniformBuffers()[i];
		_shadowsMaps[i] = shadowMap.GetShadowImage(i)->GetView(shadowsViewConfig);
	}

	resolveMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
	resolveMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLight", _dirLightUbos);
	resolveMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLightShadowMat", _shadowsMatricesUbos);
	resolveMaterialInstance->GetSlot("global")->SetGpuImages("dirLightShadowMap", _shadowsMaps);
	resolveMaterialInstance->GetSlot("global")->FlushUpdate();
}

void PbrDeferredRenderSystem::UpdateResolveMaterial() {
	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_MipLevelRanged(0, 0);
	const IGpuImageView* images[3]{};

	GpuImageViewConfig depthView = viewConfig;
	depthView.channel = SampledChannel::DEPTH;

	for (TSize i = 0; i < 3; i++) images[i] = gBuffer.GetImage(i, GBuffer::Target::DEPTH)->GetView(depthView);
	resolveMaterialInstance->GetSlot("gbuffer")->SetGpuImages("depthTexture", images);

	for (TSize i = 0; i < 3; i++) images[i] = gBuffer.GetImage(i, GBuffer::Target::COLOR)->GetView(viewConfig);
	resolveMaterialInstance->GetSlot("gbuffer")->SetGpuImages("colorTexture", images);

	for (TSize i = 0; i < 3; i++) images[i] = gBuffer.GetImage(i, GBuffer::Target::NORMAL)->GetView(viewConfig);
	resolveMaterialInstance->GetSlot("gbuffer")->SetGpuImages("normalTexture", images);

	for (TSize i = 0; i < 3; i++) images[i] = gBuffer.GetImage(i, GBuffer::Target::METALLIC_ROUGHNESS)->GetView(viewConfig);
	resolveMaterialInstance->GetSlot("gbuffer")->SetGpuImages("metallicRoughnessTexture", images);

	resolveMaterialInstance->GetSlot("gbuffer")->FlushUpdate();

	GpuImageViewConfig computeOutputConfig = GpuImageViewConfig::CreateStorage_Default();

	const IGpuImageView* _resolveImages[3]{};
	for (TSize i = 0; i < 3; i++) _resolveImages[i] = resolveRenderTarget.GetTargetImage(i)->GetView(computeOutputConfig);

	resolveMaterialInstance->GetSlot("output")->SetStorageImages("finalImage", _resolveImages);
	resolveMaterialInstance->GetSlot("output")->FlushUpdate();
}

void PbrDeferredRenderSystem::CreateTargetImage(const Vector2ui& size) {
	// GBuffer.
	gBuffer.Create(size);

	// Resolve
	RenderTargetAttachmentInfo resolveColorAttachment{};
	resolveColorAttachment.format = Format::RGBA16_SFLOAT;
	resolveColorAttachment.usage = GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::COMPUTE;
	resolveColorAttachment.name = "Resolve Target";

	resolveRenderTarget.Create(size, resolveColorAttachment);

	// Imagen final.
	RenderTargetAttachmentInfo colorAttachment{};
	colorAttachment.format = Format::RGBA16_SFLOAT;
	colorAttachment.usage = GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::COMPUTE;
	colorAttachment.name = "Deferred Target";

	RenderTargetAttachmentInfo depthAttachment{};
	depthAttachment.format = Format::D32_SFLOAT;
	depthAttachment.name = "Deferred Target Depth";

	renderTarget.Create(size, { colorAttachment }, depthAttachment);

	// TAA
	const GpuImage* sourceImages[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuImage* motionImages[NUM_RESOURCES_IN_FLIGHT]{};

	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		sourceImages[i] = resolveRenderTarget.GetTargetImage(i);
		motionImages[i] = gBuffer.GetImage(i, GBuffer::Target::MOTION);
	}

	taaProvider.InitializeTaa(
		size,
		sourceImages,
		motionImages);

	UpdateResolveMaterial();
}

void PbrDeferredRenderSystem::Resize(const Vector2ui& windowSize) {
	gBuffer.Resize(windowSize);
	resolveRenderTarget.Resize(windowSize);

	// TAA
	const GpuImage* sourceImages[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuImage* motionImages[NUM_RESOURCES_IN_FLIGHT]{};

	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		sourceImages[i] = resolveRenderTarget.GetTargetImage(i);
		motionImages[i] = gBuffer.GetImage(i, GBuffer::Target::MOTION);
	}

	taaProvider.ResizeTaa(
		windowSize,
		sourceImages,
		motionImages);

	IRenderSystem::Resize(windowSize);

	UpdateResolveMaterial();
}

void PbrDeferredRenderSystem::Render(ICommandList* commandList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	const CameraComponent3D& camera = Engine::GetEcs()->GetComponent<CameraComponent3D>(cameraObject);
	const Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(cameraObject);

	CameraInfo currentCameraInfo{};
	currentCameraInfo.projectionMatrix = camera.GetProjectionMatrix();
	currentCameraInfo.viewMatrix = camera.GetViewMatrix(cameraTransform);
	currentCameraInfo.projectionViewMatrix = currentCameraInfo.projectionMatrix * currentCameraInfo.viewMatrix;
	currentCameraInfo.position = glm::vec4(cameraTransform.GetPosition().ToGLM(), 1.0f);
	currentCameraInfo.nearFarPlanes = { camera.GetNearPlane(), camera.GetFarPlane() };

	PreviousCameraInfo previousCameraInfo{};
	previousCameraInfo.projectionMatrix = previousCameraProjection;
	previousCameraInfo.viewMatrix = previousCameraView;
	previousCameraInfo.projectionViewMatrix = previousCameraProjection * previousCameraView;

	cameraUbos[resourceIndex]->MapMemory();
	cameraUbos[resourceIndex]->Write(currentCameraInfo);
	cameraUbos[resourceIndex]->Unmap();

	previousCameraUbos[resourceIndex]->MapMemory();
	previousCameraUbos[resourceIndex]->Write(previousCameraInfo);
	previousCameraUbos[resourceIndex]->Unmap();

	previousCameraProjection = camera.GetProjectionMatrix();
	previousCameraView = camera.GetViewMatrix(cameraTransform);

	dirLightUbos[resourceIndex]->MapMemory();
	dirLightUbos[resourceIndex]->Write(dirLight);
	dirLightUbos[resourceIndex]->Unmap();

	shadowMap.SetDirectionalLight(dirLight);
		
	GenerateShadows(commandList);

	commandList->StartDebugSection("PBR Deferred", Color::RED());
	
	RenderGBuffer(commandList);
	ResolveGBuffer(commandList);

	commandList->EndDebugSection();

	ExecuteTaa(commandList);
	CopyFinalImages(commandList);


	for (const GameObjectIndex obj : GetObjects())
		previousModelMatrices.Insert(obj, Engine::GetEcs()->GetComponent<Transform3D>(obj).GetAsMatrix());

}

void PbrDeferredRenderSystem::GenerateShadows(ICommandList* commandList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	const Viewport viewport{
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
		GpuImageRange{
			.baseLayer = 0, 
			.numLayers = shadowMap.GetNumCascades(), 
			.baseMipLevel = 0, 
			.numMipLevels = ALL_MIP_LEVELS, 
			.channel = SampledChannel::DEPTH });

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

		commandList->BeginGraphicsRenderpass({ colorInfo }, depthInfo, { 1.0f, 1.0f, 1.0f, 1.0f });

		commandList->BindMaterial(*shadowMap.GetShadowsMaterial(ASSETS::ModelType::STATIC_MESH));
		commandList->BindMaterialSlot(*shadowMap.GetShadowsMaterialInstance()->GetSlot("global"));

		ShadowsRenderLoop(ModelType::STATIC_MESH, commandList, i);
		
		commandList->BindMaterial(*shadowMap.GetShadowsMaterial(ModelType::ANIMATED_MODEL));
		commandList->BindMaterialSlot(*shadowMap.GetShadowsMaterialInstance()->GetSlot("global"));

		ShadowsRenderLoop(ModelType::ANIMATED_MODEL, commandList, i);

		commandList->EndGraphicsRenderpass();

		commandList->EndDebugSection();
	}

	commandList->EndDebugSection();
}

void PbrDeferredRenderSystem::GBufferRenderLoop(ICommandList* commandList, ModelType modelType, TIndex jitterIndex) {
	GpuBuffer* previousVertexBuffer = nullptr;
	GpuBuffer* previousIndexBuffer = nullptr;

	for (GameObjectIndex obj : GetObjects()) {
		const ModelComponent3D& model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);
		const Transform3D& transform = Engine::GetEcs()->GetComponent<Transform3D>(obj);

		if (modelType != model.GetModel()->GetType())
			continue;

		if (modelType == ModelType::STATIC_MESH)
			commandList->BindMaterial(*gbufferMaterial);
		else
			commandList->BindMaterial(*animatedGbufferMaterial);

		// Actualizamos el modelo 3D, si es necesario.
		if (previousVertexBuffer != model.GetModel()->GetVertexBuffer()) {
			commandList->BindVertexBuffer(*model.GetModel()->GetVertexBuffer());
			previousVertexBuffer = model.GetModel()->GetVertexBuffer();
		}
		if (previousIndexBuffer != model.GetModel()->GetIndexBuffer()) {
			commandList->BindIndexBuffer(*model.GetModel()->GetIndexBuffer());
			previousIndexBuffer = model.GetModel()->GetIndexBuffer();
		}

		if (modelType == ModelType::ANIMATED_MODEL)
			commandList->BindMaterialSlot(*model.GetModel()->GetAnimator()->GetMaterialInstance()->GetSlot("animation"));

		for (TSize i = 0; i < model.GetModel()->GetMeshes().GetSize(); i++) {
			commandList->BindMaterialSlot(*model.GetMeshMaterialInstance(i)->GetSlot("texture"));

			const Vector4f materialInfo {
				model.GetModel()->GetMetadata().meshesMetadata[i].metallicFactor,
				model.GetModel()->GetMetadata().meshesMetadata[i].roughnessFactor,
				(float)jitterIndex,
				0.0f
			};
			struct {
				glm::mat4 model;
				glm::mat4 previousModel;
				Vector4f materialInfo;
				Vector2f resolution;
			} modelConsts {
				.model = transform.GetAsMatrix(),
				.previousModel = previousModelMatrices.ContainsKey(obj) ? previousModelMatrices.Get(obj) : glm::mat4(1.0f),
				.materialInfo = materialInfo,
				.resolution = gBuffer.GetImage(0, GBuffer::Target::COLOR)->GetSize2D().ToVector2f()
			};
			const glm::mat4 previousModelMatrix = previousModelMatrices.ContainsKey(obj)
				? previousModelMatrices.Get(obj) : glm::mat4(1.0f);
			commandList->PushMaterialConstants("model", modelConsts);

			commandList->DrawSingleMesh(model.GetModel()->GetMeshes()[i].GetFirstIndexId(), model.GetModel()->GetMeshes()[i].GetNumberOfIndices());
		}
	}
}

void PbrDeferredRenderSystem::ShadowsRenderLoop(ModelType modelType, ICommandList* commandList, TSize cascadeIndex) {
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
			commandList->DrawSingleMesh(
				model.GetModel()->GetMeshes()[i].GetFirstIndexId(), 
				model.GetModel()->GetMeshes()[i].GetNumberOfIndices());
	}
}

void PbrDeferredRenderSystem::RenderGBuffer(ICommandList* commandList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	commandList->StartDebugSection("PBR GBuffer", Color::RED());

	gBuffer.BeginRenderpass(commandList, Color::BLACK() * 0.0f);

	SetupViewport(commandList);
	commandList->BindMaterial(*gbufferMaterial);
	commandList->BindMaterialSlot(*globalGbufferMaterialInstance->GetSlot("global"));

	static TIndex jitterIndex = 0;
	constexpr TIndex maxJitter = 4;

	jitterIndex = taaProvider.GetCurrentFrameJitterIndex();

	GBufferRenderLoop(commandList, ModelType::STATIC_MESH, jitterIndex);
	GBufferRenderLoop(commandList, ModelType::ANIMATED_MODEL, jitterIndex);

	commandList->EndGraphicsRenderpass();

	commandList->EndDebugSection();
}

void PbrDeferredRenderSystem::ResolveGBuffer(ICommandList* commandList) {
	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	commandList->StartDebugSection("PBR Resolve", Color::PURPLE());

	// Sincronización de mapa de sombras.
	commandList->SetGpuImageBarrier(
		shadowMap.GetShadowImage(resourceIndex),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ),
		GpuImageRange{
			.baseLayer = 0,
			.numLayers = shadowMap.GetNumCascades(),
			.baseMipLevel = 0,
			.numMipLevels = ALL_MIP_LEVELS,
			.channel = SampledChannel::DEPTH });

	// Sincronización de render target.
	commandList->SetGpuImageBarrier(
		resolveRenderTarget.GetTargetImage(resourceIndex),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));


	// Sincronización con todos los targets de color.
	for (TIndex t = 0; t < _countof(GBuffer::ColorTargetTypes); t++)
		commandList->SetGpuImageBarrier(
			gBuffer.GetImage(resourceIndex, GBuffer::ColorTargetTypes[t]),
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	// Sincronización con depth.
	commandList->SetGpuImageBarrier(
		gBuffer.GetImage(resourceIndex, GBuffer::Target::DEPTH),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ),
		{ .channel = SampledChannel::DEPTH });

	const Vector2ui resoulution = resolveRenderTarget.GetSize();
	const Vector2ui threadGroupSize = { 8u, 8u };
	const Vector2ui dispatchRes = resoulution / threadGroupSize + Vector2ui(1u, 1u);


	commandList->BindMaterial(*resolveMaterial);
	commandList->BindMaterialInstance(*resolveMaterialInstance);
	commandList->PushMaterialConstants("taa", static_cast<int>(taaProvider.GetCurrentFrameJitterIndex()));
	commandList->DispatchCompute({ dispatchRes.X, dispatchRes.Y, 1 });

	commandList->EndDebugSection();
}

void PbrDeferredRenderSystem::ExecuteTaa(ICommandList* commandList) {
	commandList->StartDebugSection("TAA", Color::YELLOW());

	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	GpuImage* sourceImage = resolveRenderTarget.GetTargetImage(resourceIndex);

	commandList->SetGpuImageBarrier(
		sourceImage,
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ));

	taaProvider.ExecuteTaa(commandList);

	commandList->SetGpuImageBarrier(
		taaProvider.GetTaaOutput().GetTargetImage(resourceIndex),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	commandList->EndDebugSection();
}

void PbrDeferredRenderSystem::CopyFinalImages(ICommandList* cmdList) {
	cmdList->StartDebugSection("Final Image Copy", Color::BLUE());

	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	GpuImage* sourceImage = taaProvider.IsActive()
		? taaProvider.GetTaaOutput().GetTargetImage(resourceIndex)
		: resolveRenderTarget.GetTargetImage(resourceIndex);
	GpuImage* targetImage = renderTarget.GetMainColorImage(resourceIndex);


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
		CopyImageInfo::CreateDefault2D(resolveRenderTarget.GetSize()));

	cmdList->EndDebugSection();
}

void PbrDeferredRenderSystem::OnTick(TDeltaTime deltaTime) {
	for (const GameObjectIndex obj : GetObjects()) {
		Model3D* model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj).GetModel();

		if (model->GetType() == ModelType::STATIC_MESH)
			continue;

		model->GetAnimator()->Update(deltaTime);
	}
}

void PbrDeferredRenderSystem::ToggleTaa() {
	taaProvider.ToggleActivation();
}
