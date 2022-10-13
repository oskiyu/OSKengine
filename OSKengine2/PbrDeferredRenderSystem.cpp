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

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


PbrDeferredRenderSystem::PbrDeferredRenderSystem() {
	// Signature del sistema
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


	// Material del resolve final.
	resolveMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/deferred_resolve.json");
	resolveMaterialInstance = resolveMaterial->CreateInstance().GetPointer();

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

	resolveMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
	resolveMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLight", _dirLightUbos);
	resolveMaterialInstance->GetSlot("global")->SetUniformBuffers("dirLightShadowMat", _shadowsMatricesUbos);
	resolveMaterialInstance->GetSlot("global")->SetGpuImages("dirLightShadowMap", _shadowsMaps, SampledChannel::DEPTH, SampledArrayType::ARRAY);
	

	// Material del renderizado del gbuffer.
	gbufferMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/deferred_gbuffer.json");
	animatedGbufferMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/deferred_gbuffer_anim.json");
	globalGbufferMaterialInstance = gbufferMaterial->CreateInstance().GetPointer();

	globalGbufferMaterialInstance->GetSlot("global")->SetUniformBuffers("camera", _cameraUbos);
	globalGbufferMaterialInstance->GetSlot("global")->FlushUpdate();
}

void PbrDeferredRenderSystem::Initialize(GameObjectIndex camera, const ASSETS::IrradianceMap& irradianceMap) {
	cameraObject = camera;

	resolveMaterialInstance->GetSlot("global")->SetUniformBuffer("camera2D", Engine::GetRenderer()->GetRenderTargetsCamera().GetUniformBuffer());
	resolveMaterialInstance->GetSlot("global")->SetGpuImage("irradianceMap", irradianceMap.GetGpuImage());
	resolveMaterialInstance->GetSlot("global")->FlushUpdate();

	shadowMap.SetSceneCamera(camera);

	hasBeenInitialized = true;
}

void PbrDeferredRenderSystem::UpdateResolveMaterial() {
	const GpuImage* images[3]{};
	for (TSize i = 0; i < 3; i++) images[i] = gBuffer.GetTargetImages(i)[GBUFFER_POSITION_TARGET_INDEX];
	resolveMaterialInstance->GetSlot("gbuffer")->SetGpuImages("positionTexture", images);

	for (TSize i = 0; i < 3; i++) images[i] = gBuffer.GetTargetImages(i)[GBUFFER_COLOR_TARGET_INDEX];
	resolveMaterialInstance->GetSlot("gbuffer")->SetGpuImages("colorTexture", images);

	for (TSize i = 0; i < 3; i++) images[i] = gBuffer.GetTargetImages(i)[GBUFFER_NORMAL_TARGET_INDEX];
	resolveMaterialInstance->GetSlot("gbuffer")->SetGpuImages("normalTexture", images);

	resolveMaterialInstance->GetSlot("gbuffer")->FlushUpdate();
}

void PbrDeferredRenderSystem::CreateTargetImage(const Vector2ui& size) {
	// GBuffer.
	gBuffer.Create(size, Format::RGBA32_SFLOAT, Format::D32S8_SFLOAT_SUINT);  // POSITION
	gBuffer.AddColorTarget(Format::RGBA8_SRGB); // COLOR
	gBuffer.AddColorTarget(Format::RGBA32_SFLOAT); // NORMAL
	gBuffer.SetName("GBuffer");

	// Imagen final.
	renderTarget.Create(size, Format::RGBA32_SFLOAT, Format::D32S8_SFLOAT_SUINT);
	renderTarget.SetName("Deferred Target");

	UpdateResolveMaterial();
}

void PbrDeferredRenderSystem::Resize(const Vector2ui& windowSize) {
	gBuffer.Resize(windowSize);

	IRenderSystem::Resize(windowSize);

	UpdateResolveMaterial();
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

	dirLightUbos[resourceIndex]->MapMemory();
	dirLightUbos[resourceIndex]->Write(dirLight);
	dirLightUbos[resourceIndex]->Unmap();

	shadowMap.SetDirectionalLight(dirLight);

	GenerateShadows(commandList);
	RenderGBuffer(commandList);
	Resolve(commandList);
}

void PbrDeferredRenderSystem::GenerateShadows(ICommandList* commandList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	const Viewport viewport{
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

		commandList->BindMaterial(shadowMap.GetShadowsMaterial(ASSETS::ModelType::STATIC_MESH));
		commandList->BindMaterialSlot(shadowMap.GetShadowsMaterialInstance()->GetSlot("global"));

		ShadowsRenderLoop(ModelType::STATIC_MESH, commandList, i);
		
		commandList->BindMaterial(shadowMap.GetShadowsMaterial(ModelType::ANIMATED_MODEL));
		commandList->BindMaterialSlot(shadowMap.GetShadowsMaterialInstance()->GetSlot("global"));
		ShadowsRenderLoop(ModelType::ANIMATED_MODEL, commandList, i);

		commandList->EndGraphicsRenderpass();
	}

	commandList->SetGpuImageBarrier(shadowMap.GetShadowImage(resourceIndex), GpuImageLayout::SHADER_READ_ONLY,
		GpuBarrierInfo(GpuBarrierStage::DEPTH_STENCIL_END, GpuBarrierAccessStage::DEPTH_STENCIL_READ | GpuBarrierAccessStage::DEPTH_STENCIL_WRITE), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = shadowMap.GetNumCascades(), .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS, .channel = SampledChannel::DEPTH | SampledChannel::STENCIL });
}

void PbrDeferredRenderSystem::GBufferRenderLoop(GRAPHICS::ICommandList* commandList, ASSETS::ModelType modelType) {
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
				0.0f,
				0.0f
			};
			struct {
				glm::mat4 model;
				glm::mat4 transposedInverseModel;
				Vector4f materialInfo;
			} modelConsts{
				.model = transform.GetAsMatrix(),
				.transposedInverseModel = glm::transpose(glm::inverse(transform.GetAsMatrix())),
				.materialInfo = materialInfo
			};
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
			commandList->DrawSingleMesh(model.GetModel()->GetMeshes()[i].GetFirstIndexId(), model.GetModel()->GetMeshes()[i].GetNumberOfIndices());
	}
}

void PbrDeferredRenderSystem::RenderGBuffer(ICommandList* commandList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();
	
	// Sincronización con todos los targets de color sobre los que vamos a escribir.
	for (auto targetImage : gBuffer.GetTargetImages(resourceIndex))
		commandList->SetGpuImageBarrier(targetImage, GpuImageLayout::SHADER_READ_ONLY,
			GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT, GpuBarrierAccessStage::COLOR_ATTACHMENT_WRITE),
			GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });


	commandList->BeginGraphicsRenderpass(&gBuffer, Color::BLACK() * 0.0f);
	SetupViewport(commandList);
	commandList->BindMaterial(gbufferMaterial);
	commandList->BindMaterialSlot(globalGbufferMaterialInstance->GetSlot("global"));

	GBufferRenderLoop(commandList, ModelType::STATIC_MESH);
	GBufferRenderLoop(commandList, ModelType::ANIMATED_MODEL);

	commandList->EndGraphicsRenderpass();

	// Sincronización con todos los targets de color sobre los que vamos a leer en la fase resolve.
	for (auto targetImage : gBuffer.GetTargetImages(resourceIndex))
		commandList->SetGpuImageBarrier(targetImage, GpuImageLayout::SHADER_READ_ONLY,
			GpuBarrierInfo(GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT, GpuBarrierAccessStage::COLOR_ATTACHMENT_WRITE), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
			GpuImageBarrierInfo{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
}

void PbrDeferredRenderSystem::Resolve(ICommandList* cmdList) {
	cmdList->BeginGraphicsRenderpass(&renderTarget, Color::BLACK() * 0.0f);
	SetupViewport(cmdList);

	cmdList->BindVertexBuffer(Sprite::globalVertexBuffer);
	cmdList->BindIndexBuffer(Sprite::globalIndexBuffer);

	cmdList->BindMaterial(resolveMaterial);
	cmdList->BindMaterialSlot(resolveMaterialInstance->GetSlot("global"));
	cmdList->BindMaterialSlot(resolveMaterialInstance->GetSlot("gbuffer"));

	PushConst2D pushConstants{};
	pushConstants.color = Color::WHITE();
	pushConstants.matrix = renderTarget.GetSpriteTransform().GetAsMatrix();
	pushConstants.texCoords = { -1.0f, -1.0f, 1.0f, 1.0f };
	cmdList->PushMaterialConstants("sprite", pushConstants);

	cmdList->DrawSingleInstance(6);

	cmdList->EndGraphicsRenderpass();
}

void PbrDeferredRenderSystem::OnTick(TDeltaTime deltaTime) {
	for (const GameObjectIndex obj : GetObjects()) {
		Model3D* model = Engine::GetEcs()->GetComponent<ModelComponent3D>(obj).GetModel();

		if (model->GetType() == ModelType::STATIC_MESH)
			continue;

		model->GetAnimator()->Update(deltaTime);
	}
}
