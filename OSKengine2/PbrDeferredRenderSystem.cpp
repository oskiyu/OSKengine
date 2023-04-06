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
	

	// Material del resolve final.
	resolveMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/Deferred/deferred_resolve.json");
	resolveMaterialInstance = resolveMaterial->CreateInstance().GetPointer();

	// PBR
	const IGpuUniformBuffer* _cameraUbos[3]{};
	const IGpuUniformBuffer* _previousCameraUbos[NUM_RESOURCES_IN_FLIGHT]{};
	const IGpuUniformBuffer* _dirLightUbos[3]{};
	const IGpuUniformBuffer* _shadowsMatricesUbos[3]{};
	const IGpuImageView* _shadowsMaps[3]{};

	GpuImageViewConfig shadowsViewConfig = GpuImageViewConfig::CreateSampled_Array(0, shadowMap.GetNumCascades());
	shadowsViewConfig.channel = SampledChannel::DEPTH;
	
	for (TSize i = 0; i < 3; i++) {
		cameraUbos[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2 + sizeof(glm::vec4)).GetPointer();
		_cameraUbos[i] = cameraUbos[i].GetPointer();

		previousCameraUbos[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2).GetPointer();
		_previousCameraUbos[i] = previousCameraUbos[i].GetPointer();

		dirLightUbos[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(DirectionalLight)).GetPointer();
		_dirLightUbos[i] = dirLightUbos[i].GetPointer();

		_shadowsMatricesUbos[i] = shadowMap.GetDirLightMatrixUniformBuffers()[i];
		_shadowsMaps[i] = shadowMap.GetShadowImage(i)->GetView(shadowsViewConfig);
	}

	resolveMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
	resolveMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLight", _dirLightUbos);
	resolveMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLightShadowMat", _shadowsMatricesUbos);
	resolveMaterialInstance->GetSlot("global")->SetGpuImages("dirLightShadowMap", _shadowsMaps);

	// Material del renderizado del gbuffer.
	gbufferMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/Deferred/deferred_gbuffer.json");
	animatedGbufferMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/Deferred/deferred_gbuffer_anim.json");
	globalGbufferMaterialInstance = gbufferMaterial->CreateInstance().GetPointer();

	globalGbufferMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
	globalGbufferMaterialInstance->GetSlot("global")->SetUniformBuffers("previousCamera", _previousCameraUbos); /// @todo Previous camera
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

void PbrDeferredRenderSystem::UpdateResolveMaterial() {
	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_MipLevelRanged(0, 0);
	const IGpuImageView* images[3]{};
	for (TSize i = 0; i < 3; i++) images[i] = gBuffer.GetImage(i, GBuffer::Target::POSITION)->GetView(viewConfig);
	resolveMaterialInstance->GetSlot("gbuffer")->SetGpuImages("positionTexture", images);

	for (TSize i = 0; i < 3; i++) images[i] = gBuffer.GetImage(i, GBuffer::Target::COLOR)->GetView(viewConfig);
	resolveMaterialInstance->GetSlot("gbuffer")->SetGpuImages("colorTexture", images);

	for (TSize i = 0; i < 3; i++) images[i] = gBuffer.GetImage(i, GBuffer::Target::NORMAL)->GetView(viewConfig);
	resolveMaterialInstance->GetSlot("gbuffer")->SetGpuImages("normalTexture", images);

	resolveMaterialInstance->GetSlot("gbuffer")->FlushUpdate();

	GpuImageViewConfig computeOutputConfig = GpuImageViewConfig::CreateStorage_Default();

	const IGpuImageView* _resolveImages[3]{};
	for (TSize i = 0; i < 3; i++) _resolveImages[i] = resolveRenderTarget.GetTargetImage(i)->GetView(computeOutputConfig);

	resolveMaterialInstance->GetSlot("output")->SetStorageImages("finalImage", _resolveImages);
	resolveMaterialInstance->GetSlot("output")->FlushUpdate();
}

void PbrDeferredRenderSystem::UpdateTaaMaterial() {
	/*const IGpuImageView* _taaHistoricalImages[3]{};
	const IGpuImageView* _taaCurrentImages[3]{};
	const IGpuImageView* _taaMotionImages[3]{};
	const IGpuImageView* _taaTargetImages[3]{};

	const IGpuImageView* _taaDepthImages[3]{};
	const IGpuImageView* _taaHistoricalDepthImages[3]{};

	const IGpuImageView* _taaSharpenInput[3]{};
	const IGpuImageView* _taaSharpenOutput[3]{};

	GpuImageViewConfig sampledViewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	GpuImageViewConfig storageViewConfig = GpuImageViewConfig::CreateStorage_Default();
	GpuImageViewConfig depthViewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	depthViewConfig.channel = SampledChannel::DEPTH;

	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		const TIndex previousIndex = (i + NUM_RESOURCES_IN_FLIGHT - 1) % NUM_RESOURCES_IN_FLIGHT;

		_taaHistoricalImages[i] = taaRenderTarget.GetTargetImage(previousIndex)->GetView(sampledViewConfig);
		_taaCurrentImages[i] = resolveRenderTarget.GetTargetImage(i)->GetView(sampledViewConfig);
		_taaMotionImages[i] = gBuffer.GetImage(i, GBuffer::Target::MOTION)->GetView(sampledViewConfig);
		_taaTargetImages[i] = taaRenderTarget.GetTargetImage(i)->GetView(storageViewConfig);
		_taaDepthImages[i] = gBuffer.GetImage(i, GBuffer::Target::DEPTH)->GetView(depthViewConfig);
		_taaHistoricalDepthImages[i] = gBuffer.GetImage(previousIndex, GBuffer::Target::DEPTH)->GetView(depthViewConfig);

		_taaSharpenInput[i] = taaRenderTarget.GetTargetImage(i)->GetView(sampledViewConfig);
		_taaSharpenOutput[i] = taaSharpenedRenderTarget.GetTargetImage(i)->GetView(storageViewConfig);
	}

	taaMaterialInstance->GetSlot("global")->SetGpuImages("sceneImage", _taaCurrentImages);
	taaMaterialInstance->GetSlot("global")->SetGpuImages("historicalImage", _taaHistoricalImages);
	taaMaterialInstance->GetSlot("global")->SetGpuImages("velocityImage", _taaMotionImages);
	taaMaterialInstance->GetSlot("global")->SetStorageImages("finalImg", _taaTargetImages);
	taaMaterialInstance->GetSlot("global")->SetGpuImages("currentDepth", _taaDepthImages);
	taaMaterialInstance->GetSlot("global")->SetGpuImages("historicalDepth", _taaHistoricalDepthImages);
	taaMaterialInstance->GetSlot("global")->FlushUpdate();

	taaSharpenMaterialInstance->GetSlot("global")->SetGpuImages("taaImage", _taaSharpenInput);
	taaSharpenMaterialInstance->GetSlot("global")->SetStorageImages("finalImg", _taaSharpenOutput);
	taaSharpenMaterialInstance->GetSlot("global")->FlushUpdate();*/
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
	depthAttachment.format = Format::D32S8_SFLOAT_SUINT;
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
	UpdateTaaMaterial();
}

void PbrDeferredRenderSystem::Render(ICommandList* commandList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	const CameraComponent3D& camera = Engine::GetEcs()->GetComponent<CameraComponent3D>(cameraObject);
	const Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(cameraObject);

	cameraUbos[resourceIndex]->MapMemory();
	cameraUbos[resourceIndex]->Write(camera.GetProjectionMatrix());
	cameraUbos[resourceIndex]->Write(camera.GetViewMatrix(cameraTransform));
	cameraUbos[resourceIndex]->Write(cameraTransform.GetPosition());
	cameraUbos[resourceIndex]->Unmap();

	previousCameraUbos[resourceIndex]->MapMemory();
	previousCameraUbos[resourceIndex]->Write(previousCameraProjection);
	previousCameraUbos[resourceIndex]->Write(previousCameraView);
	previousCameraUbos[resourceIndex]->Unmap();

	previousCameraProjection = camera.GetProjectionMatrix();
	previousCameraView = camera.GetViewMatrix(cameraTransform);

	dirLightUbos[resourceIndex]->MapMemory();
	dirLightUbos[resourceIndex]->Write(dirLight);
	dirLightUbos[resourceIndex]->Unmap();

	shadowMap.SetDirectionalLight(dirLight);

	commandList->StartDebugSection("PBR Deferred", Color::RED());
	
	GenerateShadows(commandList);
	RenderGBuffer(commandList);
	ResolveGBuffer(commandList);
	ExecuteTaa(commandList);
	CopyFinalImages(commandList);

	for (const GameObjectIndex obj : GetObjects())
		previousModelMatrices.Insert(obj, Engine::GetEcs()->GetComponent<Transform3D>(obj).GetAsMatrix());

	commandList->EndDebugSection();
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
		GpuImageLayout::DEPTH_STENCIL_TARGET,
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), 
		GpuBarrierInfo(GpuBarrierStage::DEPTH_STENCIL_START, GpuBarrierAccessStage::DEPTH_STENCIL_READ | GpuBarrierAccessStage::DEPTH_STENCIL_WRITE),
		GpuImageBarrierInfo{ 
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

		commandList->BindMaterial(shadowMap.GetShadowsMaterial(ASSETS::ModelType::STATIC_MESH));
		commandList->BindMaterialSlot(shadowMap.GetShadowsMaterialInstance()->GetSlot("global"));

		ShadowsRenderLoop(ModelType::STATIC_MESH, commandList, i);
		
		commandList->BindMaterial(shadowMap.GetShadowsMaterial(ModelType::ANIMATED_MODEL));
		commandList->BindMaterialSlot(shadowMap.GetShadowsMaterialInstance()->GetSlot("global"));

		ShadowsRenderLoop(ModelType::ANIMATED_MODEL, commandList, i);

		commandList->EndGraphicsRenderpass();

		commandList->EndDebugSection();
	}

	commandList->SetGpuImageBarrier(
		shadowMap.GetShadowImage(resourceIndex), 
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::DEPTH_STENCIL_END, GpuBarrierAccessStage::DEPTH_STENCIL_READ | GpuBarrierAccessStage::DEPTH_STENCIL_WRITE), 
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		GpuImageBarrierInfo{ 
			.baseLayer = 0, 
			.numLayers = shadowMap.GetNumCascades(), 
			.baseMipLevel = 0, 
			.numMipLevels = ALL_MIP_LEVELS, 
			.channel = SampledChannel::DEPTH });

	commandList->EndDebugSection();
}

void PbrDeferredRenderSystem::GBufferRenderLoop(ICommandList* commandList, ModelType modelType, TIndex jitterIndex) {
	IGpuVertexBuffer* previousVertexBuffer = nullptr;
	IGpuIndexBuffer* previousIndexBuffer = nullptr;

	for (GameObjectIndex obj : GetObjects()) {
		const ModelComponent3D& model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj);
		const Transform3D& transform = Engine::GetEcs()->GetComponent<Transform3D>(obj);

		if (modelType != model.GetModel()->GetType())
			continue;

		if (modelType == ModelType::STATIC_MESH)
			commandList->BindMaterial(gbufferMaterial);
		else
			commandList->BindMaterial(animatedGbufferMaterial);

		// Actualizamos el modelo 3D, si es necesario.
		if (previousVertexBuffer != model.GetModel()->GetVertexBuffer()) {
			commandList->BindVertexBuffer(model.GetModel()->GetVertexBuffer());
			previousVertexBuffer = model.GetModel()->GetVertexBuffer();
		}
		if (previousIndexBuffer != model.GetModel()->GetIndexBuffer()) {
			commandList->BindIndexBuffer(model.GetModel()->GetIndexBuffer());
			previousIndexBuffer = model.GetModel()->GetIndexBuffer();
		}

		if (modelType == ModelType::ANIMATED_MODEL)
			commandList->BindMaterialSlot(model.GetModel()->GetAnimator()->GetMaterialInstance()->GetSlot("animation"));

		for (TSize i = 0; i < model.GetModel()->GetMeshes().GetSize(); i++) {
			commandList->BindMaterialSlot(model.GetMeshMaterialInstance(i)->GetSlot("texture"));

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
			commandList->DrawSingleMesh(
				model.GetModel()->GetMeshes()[i].GetFirstIndexId(), 
				model.GetModel()->GetMeshes()[i].GetNumberOfIndices());
	}
}

void PbrDeferredRenderSystem::RenderGBuffer(ICommandList* commandList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	commandList->StartDebugSection("PBR GBuffer", Color::RED());

	// Sincronización con todos los targets de color sobre los que vamos a escribir.
	for (TIndex t = 0; t < _countof(GBuffer::ColorTargetTypes); t++)
		commandList->SetGpuImageBarrier(
			gBuffer.GetImage(resourceIndex, GBuffer::ColorTargetTypes[t]),
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
			GpuBarrierInfo(GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT, GpuBarrierAccessStage::COLOR_ATTACHMENT_WRITE));

	// Depth
	commandList->SetGpuImageBarrier(
		gBuffer.GetImage(resourceIndex, GBuffer::Target::DEPTH),
		GpuImageLayout::DEPTH_STENCIL_TARGET,
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		GpuBarrierInfo(GpuBarrierStage::DEPTH_STENCIL_END, GpuBarrierAccessStage::DEPTH_STENCIL_READ | GpuBarrierAccessStage::DEPTH_STENCIL_WRITE),
		{ .channel = SampledChannel::DEPTH });

	gBuffer.BeginRenderpass(commandList, Color::BLACK() * 0.0f);
	SetupViewport(commandList);
	commandList->BindMaterial(gbufferMaterial);
	commandList->BindMaterialSlot(globalGbufferMaterialInstance->GetSlot("global"));

	static TIndex jitterIndex = 0;
	constexpr TIndex maxJitter = 4;

	jitterIndex = taaProvider.GetCurrentFrameJitterIndex();

	GBufferRenderLoop(commandList, ModelType::STATIC_MESH, jitterIndex);
	GBufferRenderLoop(commandList, ModelType::ANIMATED_MODEL, jitterIndex);

	commandList->EndGraphicsRenderpass();

	// Sincronización con todos los targets de color sobre los que vamos a leer en la fase resolve.
	for (TIndex t = 0; t < _countof(GBuffer::ColorTargetTypes); t++)
		commandList->SetGpuImageBarrier(
			gBuffer.GetImage(resourceIndex, GBuffer::ColorTargetTypes[t]), 
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT, GpuBarrierAccessStage::COLOR_ATTACHMENT_WRITE), 
			GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ));

	// Depth
	commandList->SetGpuImageBarrier(
		gBuffer.GetImage(resourceIndex, GBuffer::Target::DEPTH),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::DEPTH_STENCIL_START, GpuBarrierAccessStage::DEPTH_STENCIL_READ | GpuBarrierAccessStage::DEPTH_STENCIL_WRITE),
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		{ .channel = SampledChannel::DEPTH });

	commandList->EndDebugSection();
}

void PbrDeferredRenderSystem::ResolveGBuffer(ICommandList* commandList) {
	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	commandList->StartDebugSection("PBR Resolve", Color::RED());

	commandList->SetGpuImageBarrier(
		resolveRenderTarget.GetTargetImage(resourceIndex),
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ),
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE));

	const Vector2ui resoulution = resolveRenderTarget.GetSize();
	const Vector2ui threadGroupSize = { 8u, 8u };
	const Vector2ui dispatchRes = resoulution / threadGroupSize + Vector2ui(1u, 1u);

	commandList->BindMaterial(resolveMaterial);
	commandList->BindMaterialSlot(resolveMaterialInstance->GetSlot("global"));
	commandList->BindMaterialSlot(resolveMaterialInstance->GetSlot("gbuffer"));
	commandList->BindMaterialSlot(resolveMaterialInstance->GetSlot("output"));
	commandList->DispatchCompute({ dispatchRes.X, dispatchRes.Y, 1 });

	commandList->SetGpuImageBarrier(
		resolveRenderTarget.GetTargetImage(resourceIndex),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE),
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ));

	commandList->EndDebugSection();
}

void PbrDeferredRenderSystem::ExecuteTaa(ICommandList* commandList) {
	commandList->StartDebugSection("TAA", Color::YELLOW());

	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	GpuImage* sourceImage = resolveRenderTarget.GetTargetImage(resourceIndex);

	commandList->SetGpuImageBarrier(
		sourceImage,
		TaaProvider::GetTaaSourceLayout(),
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ),
		TaaProvider::GetTaaSourceBarrierInfo());
	
	taaProvider.ExecuteTaa(commandList);

	commandList->SetGpuImageBarrier(
		taaProvider.GetTaaOutput().GetTargetImage(resourceIndex),
		GpuImageLayout::SAMPLED,
		TaaProvider::GetTaaOutputBarrierInfo(),
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ));

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
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ),
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_READ));
	
	// Imagen del render target final: transfer destination.
	cmdList->SetGpuImageBarrier(
		targetImage,
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE));

	cmdList->CopyImageToImage(
		sourceImage,
		targetImage,
		CopyImageInfo::CreateDefault2D(resolveRenderTarget.GetSize()));

	// Imagen del render target final: sampled.
	cmdList->SetGpuImageBarrier(
		targetImage,
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE),
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ));

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
