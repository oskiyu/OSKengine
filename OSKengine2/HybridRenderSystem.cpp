#include "HybridRenderSystem.h"

#include "OSKengine.h"
#include "EntityComponentSystem.h"

#include "Transform3D.h"
#include "ModelComponent3D.h"
#include "CameraComponent3D.h"

#include "Material.h"
#include "GpuImageLayout.h"
#include "IrradianceMap.h"
#include "SpecularMap.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

HybridRenderSystem::HybridRenderSystem() {
	// Signature del sistema
	Signature signature{};
	signature.SetTrue(Engine::GetEcs()->GetComponentType<Transform3D>());
	signature.SetTrue(Engine::GetEcs()->GetComponentType<ModelComponent3D>());
	_SetSignature(signature);

	// Directional light por defecto
	const Vector3f direction = Vector3f(1.0f, -3.f, 0.0f).GetNormalized();
	dirLight.directionAndIntensity = Vector4f(direction.X, direction.Y, direction.Z, 1.2f);
	dirLight.color = Color(253 / 255.f, 253 / 255.f, 225 / 255.f);

	SetupRtResources();
	SetupGBufferResources();

	// Shadows
	shadowsMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/Hybrid/shadows.json");
	shadowsMaterialInstance = shadowsMaterial->CreateInstance().GetPointer();

	shadowsReprojectionMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/Hybrid/shadows_reprojection.json");
	shadowsReprojectionMaterialInstance = shadowsReprojectionMaterial->CreateInstance().GetPointer();

	shadowsAtrousMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/Hybrid/shadows_denoise.json");
	shadowsAtrousMaterialInstance = shadowsAtrousMaterial->CreateInstance().GetPointer();

	// Resolve
	resolveMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/Hybrid/resolve.json");
	resolveMaterialInstance = resolveMaterial->CreateInstance().GetPointer();
}

void HybridRenderSystem::SetupGBufferResources() {
	gbufferMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/deferred_gbuffer.json");
	animatedGbufferMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/deferred_gbuffer_anim.json");
	globalGbufferMaterialInstance = gbufferMaterial->CreateInstance().GetPointer();

	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		cameraUbos[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2 + sizeof(glm::vec4)).GetPointer();
		previousCameraUbos[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(glm::mat4) * 2).GetPointer();
	}

	SetupGBufferInstance();
}

void HybridRenderSystem::SetupRtResources() {
	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		topLevelAccelerationStructures[i] = Engine::GetRenderer()->GetAllocator()->CreateTopAccelerationStructure({}).GetPointer();
		dirLightUbos[i] = Engine::GetRenderer()->GetAllocator()->CreateUniformBuffer(sizeof(DirectionalLight)).GetPointer();
	}

	noise = Engine::GetAssetManager()->Load<Texture>("Resources/Assets/BlueNoise/bluenoise.json", "GLOBAL");
	noiseX = Engine::GetAssetManager()->Load<Texture>("Resources/Assets/BlueNoise/bluenoisex.json", "GLOBAL");
}

void HybridRenderSystem::SetupGBufferInstance() {
	const IGpuUniformBuffer* _cameraUbos[3]{};
	const IGpuUniformBuffer* _prevCameraUbos[3]{};
	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		_cameraUbos[i] = cameraUbos[i].GetPointer();
		_prevCameraUbos[i] = previousCameraUbos[i].GetPointer();
	}

	globalGbufferMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
	globalGbufferMaterialInstance->GetSlot("global")->SetUniformBuffers("previousCamera", _prevCameraUbos);
	globalGbufferMaterialInstance->GetSlot("global")->FlushUpdate();
}

void HybridRenderSystem::SetupShadowsInstance() {
	const ITopLevelAccelerationStructure* tlas[NUM_RESOURCES_IN_FLIGHT]{};
	
	const GpuImage* raytracedImgs[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuImage* reprojectedImgs[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuImage* denoisedImgs[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuImage* historicalImgs[NUM_RESOURCES_IN_FLIGHT]{};

	const GpuImage* positionImgs[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuImage* velocityImgs[NUM_RESOURCES_IN_FLIGHT]{};
	const IGpuUniformBuffer* _dirLightUbos[NUM_RESOURCES_IN_FLIGHT]{};

	const GpuImage* currentGbufferPositionImgs[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuImage* currentGbufferNormalImgs[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuImage* historicalGbufferPositionImgs[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuImage* historicalGbufferNormalImgs[NUM_RESOURCES_IN_FLIGHT]{};

	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		tlas[i] = topLevelAccelerationStructures[i].GetPointer();

		raytracedImgs[i] = raytracedShadowsTarget.GetTargetImage(i);
		reprojectedImgs[i] = reprojectedShadowsTarget.GetTargetImage(i);
		denoisedImgs[i] = finalShadowsTarget.GetTargetImage(i);
		historicalImgs[i] = historicalShadowsTarget.GetTargetImage(i);

		positionImgs[i] = gBuffer.GetImage(i, GBuffer::Target::POSITION);
		velocityImgs[i] = gBuffer.GetImage(i, GBuffer::Target::MOTION);
		_dirLightUbos[i] = dirLightUbos->GetPointer();

		currentGbufferPositionImgs[i] = gBuffer.GetImage(i, GBuffer::Target::POSITION);
		currentGbufferNormalImgs[i] = gBuffer.GetImage(i, GBuffer::Target::NORMAL);
		historicalGbufferPositionImgs[i] = historicalGBuffer.GetImage(i, GBuffer::Target::POSITION);
		historicalGbufferNormalImgs[i] = historicalGBuffer.GetImage(i, GBuffer::Target::NORMAL);
	}

	shadowsMaterialInstance->GetSlot("rt")->SetAccelerationStructures("topLevelAccelerationStructure", tlas);
	shadowsMaterialInstance->GetSlot("shadows")->SetGpuImages("gBufferPosition", positionImgs);
	shadowsMaterialInstance->GetSlot("shadows")->SetStorageImages("shadowsImage", raytracedImgs);
	shadowsMaterialInstance->GetSlot("shadows")->SetUniformBuffers("dirLight", _dirLightUbos);
	shadowsMaterialInstance->GetSlot("shadows")->SetGpuImage("noiseImage", noise->GetGpuImage());
	shadowsMaterialInstance->GetSlot("shadows")->SetGpuImage("noiseImageX", noiseX->GetGpuImage());

	shadowsMaterialInstance->GetSlot("rt")->FlushUpdate();
	shadowsMaterialInstance->GetSlot("shadows")->FlushUpdate();

	shadowsReprojectionMaterialInstance->GetSlot("shadows")->SetGpuImages("raytracedShadowsImg", raytracedImgs);
	shadowsReprojectionMaterialInstance->GetSlot("shadows")->SetStorageImages("reprojectedShadowsImg", reprojectedImgs);

	shadowsReprojectionMaterialInstance->GetSlot("shadows")->SetGpuImages("historicalImg", historicalImgs);
	shadowsReprojectionMaterialInstance->GetSlot("shadows")->SetGpuImages("velocityImg", velocityImgs);

	shadowsReprojectionMaterialInstance->GetSlot("shadows")->SetGpuImages("currentPositionImg", currentGbufferPositionImgs);
	shadowsReprojectionMaterialInstance->GetSlot("shadows")->SetGpuImages("historicalPositionImg", historicalGbufferPositionImgs);
	shadowsReprojectionMaterialInstance->GetSlot("shadows")->SetGpuImages("currentNormalImg", currentGbufferNormalImgs);
	shadowsReprojectionMaterialInstance->GetSlot("shadows")->SetGpuImages("historicalNormalImg", historicalGbufferNormalImgs);

	shadowsReprojectionMaterialInstance->GetSlot("shadows")->FlushUpdate();

	shadowsAtrousMaterialInstance->GetSlot("shadows")->SetGpuImages("reprojectedImg", reprojectedImgs);
	shadowsAtrousMaterialInstance->GetSlot("shadows")->SetStorageImages("denoisedShadowsImg", denoisedImgs);

	shadowsAtrousMaterialInstance->GetSlot("shadows")->SetGpuImages("currentPositionImg", currentGbufferPositionImgs);
	shadowsAtrousMaterialInstance->GetSlot("shadows")->SetGpuImages("historicalPositionImg", historicalGbufferPositionImgs);
	shadowsAtrousMaterialInstance->GetSlot("shadows")->SetGpuImages("currentNormalImg", currentGbufferNormalImgs);
	shadowsAtrousMaterialInstance->GetSlot("shadows")->SetGpuImages("historicalNormalImg", historicalGbufferNormalImgs);

	shadowsAtrousMaterialInstance->GetSlot("shadows")->FlushUpdate();
}

void HybridRenderSystem::SetupResolveInstance() {
	const GpuImage* positionImgs[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuImage* colorImgs[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuImage* normalImgs[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuImage* shadowImgs[NUM_RESOURCES_IN_FLIGHT]{};
	const GpuImage* finalImgs[NUM_RESOURCES_IN_FLIGHT]{};
	
	const IGpuUniformBuffer* _cameraUbos[NUM_RESOURCES_IN_FLIGHT]{};
	const IGpuUniformBuffer* _dirLightUbos[NUM_RESOURCES_IN_FLIGHT]{};

	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		positionImgs[i] = gBuffer.GetImage(i, GBuffer::Target::POSITION);
		colorImgs[i] = gBuffer.GetImage(i, GBuffer::Target::COLOR);
		normalImgs[i] = gBuffer.GetImage(i, GBuffer::Target::NORMAL);

		shadowImgs[i] = finalShadowsTarget.GetTargetImage(i);
		finalImgs[i] = renderTarget.GetMainColorImage(i);

		_cameraUbos[i] = cameraUbos[i].GetPointer();
		_dirLightUbos[i] = dirLightUbos[i].GetPointer();
	}

	resolveMaterialInstance->GetSlot("targets")->SetGpuImages("positionImg", positionImgs);
	resolveMaterialInstance->GetSlot("targets")->SetGpuImages("colorImg", colorImgs);
	resolveMaterialInstance->GetSlot("targets")->SetGpuImages("normalImg", normalImgs);
	resolveMaterialInstance->GetSlot("targets")->SetGpuImages("shadowsImg", shadowImgs);
	resolveMaterialInstance->GetSlot("targets")->SetStorageImages("finalImg", finalImgs);

	resolveMaterialInstance->GetSlot("scene")->SetUniformBuffers("camera", _cameraUbos);
	resolveMaterialInstance->GetSlot("scene")->SetUniformBuffers("dirLight", _dirLightUbos);

	resolveMaterialInstance->GetSlot("targets")->FlushUpdate();
	resolveMaterialInstance->GetSlot("scene")->FlushUpdate();
}

void HybridRenderSystem::Initialize(GameObjectIndex cameraObject, const IrradianceMap& irradianceMap, const SpecularMap& specularMap) {
	this->cameraObject = cameraObject;

	resolveMaterialInstance->GetSlot("scene")->SetGpuImage("irradianceMap", irradianceMap.GetGpuImage());
	resolveMaterialInstance->GetSlot("scene")->SetGpuImage("specularMap", specularMap.GetCubemapImage());
	resolveMaterialInstance->GetSlot("scene")->SetGpuImage("specularLut", specularMap.GetLookUpTable());
	resolveMaterialInstance->GetSlot("scene")->FlushUpdate();
}

void HybridRenderSystem::CreateTargetImage(const Vector2ui& size) {
	GpuImageSamplerDesc sampler{};
	sampler.mipMapMode = GpuImageMipmapMode::NONE;
	sampler.filteringType = GpuImageFilteringType::NEAREST;

	// GBuffer.
	gBuffer.Create(size, sampler, GpuImageUsage::COMPUTE | GpuImageUsage::RT_TARGET_IMAGE | GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION);
	historicalGBuffer.Create(size, sampler, GpuImageUsage::COMPUTE | GpuImageUsage::RT_TARGET_IMAGE | GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION);

	// Imagen final.
	RenderTargetAttachmentInfo finalColorAttInfo{};
	finalColorAttInfo.format = Format::RGBA32_SFLOAT;
	finalColorAttInfo.usage = GpuImageUsage::COMPUTE;
	finalColorAttInfo.sampler = sampler;
	finalColorAttInfo.name = "Hybrid Target";

	RenderTargetAttachmentInfo finalDepthAttInfo{};
	finalDepthAttInfo.format = Format::D32S8_SFLOAT_SUINT;
	finalDepthAttInfo.sampler = sampler;
	finalDepthAttInfo.name = "Hybrid Target Depth";
	renderTarget.Create(size, { finalColorAttInfo }, finalDepthAttInfo);

	// Shadows
	RenderTargetAttachmentInfo rtShadowsAttachmentInfo{};
	rtShadowsAttachmentInfo.format = Format::RGBA8_UNORM;
	rtShadowsAttachmentInfo.sampler = sampler;
	rtShadowsAttachmentInfo.name = "RayTraced Shadows";
	raytracedShadowsTarget.Create(size, rtShadowsAttachmentInfo);

	RenderTargetAttachmentInfo denoisingAttachmentInfo{};
	denoisingAttachmentInfo.format = Format::RGBA32_SFLOAT;
	denoisingAttachmentInfo.sampler = sampler;
	denoisingAttachmentInfo.usage = GpuImageUsage::COMPUTE | GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION;
	denoisingAttachmentInfo.name = "RayTraced Reprojected Shadows";
	reprojectedShadowsTarget.Create(size, denoisingAttachmentInfo);

	denoisingAttachmentInfo.name = "RayTraced Denoised Shadows";
	finalShadowsTarget.Create(size, denoisingAttachmentInfo);

	denoisingAttachmentInfo.name = "RayTraced Previous Denoised Shadows";
	historicalShadowsTarget.Create(size, denoisingAttachmentInfo);

	SetupShadowsInstance();
	SetupResolveInstance();
}

void HybridRenderSystem::Resize(const Vector2ui& size) {
	gBuffer.Resize(size);
	historicalGBuffer.Resize(size);

	raytracedShadowsTarget.Resize(size);
	reprojectedShadowsTarget.Resize(size);
	finalShadowsTarget.Resize(size);
	historicalShadowsTarget.Resize(size);

	IRenderSystem::Resize(size);

	SetupShadowsInstance();
	SetupResolveInstance();
}


void HybridRenderSystem::GBufferRenderLoop(GRAPHICS::ICommandList* commandList, ASSETS::ModelType modelType) {
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

			const glm::mat4 previousModelMatrix = previousModelMatrices.ContainsKey(obj)
				? previousModelMatrices.Get(obj) : glm::mat4(1.0f);

			const Vector4f materialInfo{
				model.GetModel()->GetMetadata().meshesMetadata[i].metallicFactor,
				model.GetModel()->GetMetadata().meshesMetadata[i].roughnessFactor,
				0.0f,
				0.0f
			};
			struct {
				glm::mat4 model;
				glm::mat4 previousModel;
				Vector4f materialInfo;
			} modelConsts{
				.model = transform.GetAsMatrix(),
				.previousModel = previousModelMatrix,
				.materialInfo = materialInfo
			};
			commandList->PushMaterialConstants("model", modelConsts);

			commandList->DrawSingleMesh(model.GetModel()->GetMeshes()[i].GetFirstIndexId(), model.GetModel()->GetMeshes()[i].GetNumberOfIndices());
		}
	}
}

void HybridRenderSystem::RenderGBuffer(GRAPHICS::ICommandList* commandList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	// Sincronización con todos los targets de color sobre los que vamos a escribir.
	for (TIndex t = 0; t < _countof(GBuffer::ColorTargetTypes); t++)
		commandList->SetGpuImageBarrier(gBuffer.GetImage(resourceIndex, GBuffer::ColorTargetTypes[t]), GpuImageLayout::COLOR_ATTACHMENT,
			GpuBarrierInfo(GpuBarrierStage::RAYTRACING_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT, GpuBarrierAccessStage::COLOR_ATTACHMENT_WRITE),
			GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });


	gBuffer.BeginRenderpass(commandList, Color::BLACK() * 0.0f);

	SetupViewport(commandList);
	commandList->BindMaterial(gbufferMaterial);
	commandList->BindMaterialSlot(globalGbufferMaterialInstance->GetSlot("global"));

	GBufferRenderLoop(commandList, ModelType::STATIC_MESH);
	GBufferRenderLoop(commandList, ModelType::ANIMATED_MODEL);

	commandList->EndGraphicsRenderpass();


	// Copia al gbuffer historico
	for (TIndex i = 0; i < _countof(GBuffer::ColorTargetTypes); i++) {
		GpuImage* previousImage = gBuffer.GetImage((resourceIndex + NUM_RESOURCES_IN_FLIGHT - 1) % NUM_RESOURCES_IN_FLIGHT, GBuffer::ColorTargetTypes[i]);
		GpuImage* targetImage = historicalGBuffer.GetImage(resourceIndex, GBuffer::ColorTargetTypes[i]);

		commandList->SetGpuImageBarrier(previousImage, GpuImageLayout::TRANSFER_SOURCE,
			GpuBarrierInfo(GpuBarrierStage::RAYTRACING_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_READ));
		commandList->SetGpuImageBarrier(targetImage, GpuImageLayout::TRANSFER_DESTINATION,
			GpuBarrierInfo(GpuBarrierStage::RAYTRACING_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE));
		
		const Vector2ui imgSize = { targetImage->GetSize().X, targetImage->GetSize().Y };
		CopyImageInfo copyInfo = CopyImageInfo::CreateDefault2D(imgSize);
		commandList->CopyImageToImage(previousImage, targetImage, copyInfo);

		commandList->SetGpuImageBarrier(previousImage, GpuImageLayout::GENERAL,
			GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_READ), GpuBarrierInfo(GpuBarrierStage::RAYTRACING_SHADER, GpuBarrierAccessStage::SHADER_READ));
		commandList->SetGpuImageBarrier(targetImage, GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ));
	}
	

	// Sincronización con todos los targets de color sobre los que vamos a leer en la fase de trazado de rayos.
	for (TIndex t = 0; t < _countof(GBuffer::ColorTargetTypes); t++)
		commandList->SetGpuImageBarrier(gBuffer.GetImage(resourceIndex, GBuffer::ColorTargetTypes[t]), GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::RAYTRACING_SHADER, GpuBarrierAccessStage::SHADER_READ),
			GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
}

void HybridRenderSystem::RenderShadows(GRAPHICS::ICommandList* cmdList) {
	const TSize imgIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	GpuImage* rayTracedImage = raytracedShadowsTarget.GetTargetImage(imgIndex);
	GpuImage* reprojectedImage = reprojectedShadowsTarget.GetTargetImage(imgIndex);
	GpuImage* denoisedImage = finalShadowsTarget.GetTargetImage(imgIndex);
	GpuImage* previousDenoisedImage = finalShadowsTarget.GetTargetImage((imgIndex + NUM_RESOURCES_IN_FLIGHT - 1) % NUM_RESOURCES_IN_FLIGHT);
	GpuImage* historicalImage = historicalShadowsTarget.GetTargetImage(imgIndex);

	const Vector2ui resolution = finalShadowsTarget.GetSize();
	const Vector2ui threadGroupSize = { 8u, 8u };
	const Vector2ui dispatchRes = resolution / threadGroupSize + Vector2ui(1u, 1u);

	// Ray traces

	cmdList->SetGpuImageBarrier(rayTracedImage, GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), 
		GpuBarrierInfo(GpuBarrierStage::RAYTRACING_SHADER, GpuBarrierAccessStage::SHADER_WRITE));

	cmdList->BindMaterial(shadowsMaterial);
	cmdList->BindMaterialSlot(shadowsMaterialInstance->GetSlot("rt"));
	cmdList->BindMaterialSlot(shadowsMaterialInstance->GetSlot("shadows"));

	static int temporalIndex = 0;
	temporalIndex++;
	temporalIndex = temporalIndex % 64;

	cmdList->PushMaterialConstants("frameInfo", temporalIndex);

	cmdList->TraceRays(0, 0, 0, { rayTracedImage->GetSize().X, rayTracedImage->GetSize().Y });

	cmdList->SetGpuImageBarrier(rayTracedImage, GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::RAYTRACING_SHADER, GpuBarrierAccessStage::SHADER_WRITE), 
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ));


	// Copia hacia la imagen histórica del frame anterior

	cmdList->SetGpuImageBarrier(previousDenoisedImage, GpuImageLayout::TRANSFER_SOURCE,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), 
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_READ));
	cmdList->SetGpuImageBarrier(historicalImage, GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), 
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE));

	const Vector2ui imgSize = { historicalImage->GetSize().X, historicalImage->GetSize().Y };
	CopyImageInfo copyInfo = CopyImageInfo::CreateDefault2D(imgSize);
	cmdList->CopyImageToImage(previousDenoisedImage, historicalImage, copyInfo);

	cmdList->SetGpuImageBarrier(previousDenoisedImage, GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_READ), 
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ));
	cmdList->SetGpuImageBarrier(historicalImage, GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE), 
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ));
	

	// Reproyección

	cmdList->SetGpuImageBarrier(reprojectedImage, GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), 
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE));

	cmdList->BindMaterial(shadowsReprojectionMaterial);
	cmdList->BindMaterialSlot(shadowsReprojectionMaterialInstance->GetSlot("shadows"));
	cmdList->DispatchCompute({ dispatchRes.X, dispatchRes.Y, 1 });

	cmdList->SetGpuImageBarrier(reprojectedImage, GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE), 
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ));

	// Denoising

	cmdList->SetGpuImageBarrier(denoisedImage, GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), 
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE));
	
	cmdList->BindMaterial(shadowsAtrousMaterial);
	cmdList->BindMaterialSlot(shadowsAtrousMaterialInstance->GetSlot("shadows"));
	cmdList->DispatchCompute({ dispatchRes.X, dispatchRes.Y, 1 });

	cmdList->SetGpuImageBarrier(denoisedImage, GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE), 
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ));
}

void HybridRenderSystem::Resolve(GRAPHICS::ICommandList* cmdList) {
	const TSize imgIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	GpuImage* targetImage = renderTarget.GetMainColorImage(imgIndex);

	cmdList->SetGpuImageBarrier(targetImage, GpuImageLayout::SAMPLED, GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), 
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE));

	for (TIndex t = 0; t < _countof(GBuffer::ColorTargetTypes); t++)
		cmdList->SetGpuImageBarrier(gBuffer.GetImage(imgIndex, GBuffer::ColorTargetTypes[t]), GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuBarrierStage::RAYTRACING_SHADER, GpuBarrierAccessStage::SHADER_READ), 
			GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ),
			GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	const Vector2ui resoulution = renderTarget.GetSize();
	const Vector2ui threadGroupSize = { 8u, 8u };
	const Vector2ui dispatchRes = resoulution / threadGroupSize + Vector2ui(1u, 1u);
	
	cmdList->BindMaterial(resolveMaterial);
	cmdList->BindMaterialSlot(resolveMaterialInstance->GetSlot("targets"));
	cmdList->BindMaterialSlot(resolveMaterialInstance->GetSlot("scene"));
	cmdList->DispatchCompute({ dispatchRes.X, dispatchRes.Y, 1 });

	// sRGB
	for (TIndex t = 0; t < _countof(GBuffer::ColorTargetTypes); t++)
		cmdList->SetGpuImageBarrier(gBuffer.GetImage(imgIndex, GBuffer::ColorTargetTypes[t]), GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), 
			GpuBarrierInfo(GpuBarrierStage::RAYTRACING_SHADER, GpuBarrierAccessStage::SHADER_READ),
			GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	cmdList->SetGpuImageBarrier(targetImage, GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE), 
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ));
}

void HybridRenderSystem::Render(GRAPHICS::ICommandList* commandList) {
	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	topLevelAccelerationStructures[resourceIndex]->Update(commandList);

	const CameraComponent3D& camera = Engine::GetEcs()->GetComponent<CameraComponent3D>(cameraObject);
	const Transform3D& cameraTransform = Engine::GetEcs()->GetComponent<Transform3D>(cameraObject);

	cameraUbos[resourceIndex]->MapMemory();
	cameraUbos[resourceIndex]->Write(camera.GetProjectionMatrix());
	cameraUbos[resourceIndex]->Write(camera.GetViewMatrix(cameraTransform));
	cameraUbos[resourceIndex]->Write(cameraTransform.GetPosition());
	cameraUbos[resourceIndex]->Unmap();

	previousCameraUbos[resourceIndex]->MapMemory();
	previousCameraUbos[resourceIndex]->Write(previousProjection);
	previousCameraUbos[resourceIndex]->Write(previousView);
	previousCameraUbos[resourceIndex]->Unmap();

	previousProjection = camera.GetProjectionMatrix();
	previousView = camera.GetViewMatrix(cameraTransform);

	dirLightUbos[resourceIndex]->MapMemory();
	dirLightUbos[resourceIndex]->Write(dirLight);
	dirLightUbos[resourceIndex]->Unmap();

	RenderGBuffer(commandList);
	RenderShadows(commandList);
	Resolve(commandList);

	for (const GameObjectIndex obj : GetObjects()) {
		previousModelMatrices.Insert(obj, Engine::GetEcs()->GetComponent<Transform3D>(obj).GetAsMatrix());
	}
}

void HybridRenderSystem::OnTick(TDeltaTime deltaTime) {

}


void HybridRenderSystem::AddBlas(IBottomLevelAccelerationStructure* blas) {
	for (TIndex i = 0; i < _countof(topLevelAccelerationStructures); i++)
		topLevelAccelerationStructures[i]->AddBottomLevelAccelerationStructure(blas);
}
