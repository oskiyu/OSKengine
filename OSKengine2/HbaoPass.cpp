#include "HbaoPass.h"

#include "ICommandList.h"

#include "OSKengine.h"
#include "MaterialSystem.h"
#include "Material.h"

#include "AssetManager.h"
#include "Texture.h"

#include "IGpuImage.h"
#include "IGpuImageView.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void HbaoPass::SetNormalsInput(std::span<GpuImage*, NUM_RESOURCES_IN_FLIGHT> images) {
	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		m_normalInputs[i] = images[i];
		m_normalInputViews[i] = images[i]->GetView(viewConfig);
	}

	if (hbaoMaterialInstance.HasValue())
		hbaoMaterialInstance->GetSlot("texture")->SetGpuImages("normalImage", m_normalInputViews);
}

void HbaoPass::SetDepthInput(std::span<GpuImage*, NUM_RESOURCES_IN_FLIGHT> images) {
	GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	viewConfig.channel = SampledChannel::DEPTH;

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		m_depthInputs[i] = images[i];
		m_depthInputViews[i] = images[i]->GetView(viewConfig);
	}

	if (hbaoMaterialInstance.HasValue())
		hbaoMaterialInstance->GetSlot("texture")->SetGpuImages("depthImage", m_depthInputViews);

	if (blurMaterialInstanceA.HasValue())
		blurMaterialInstanceA->GetSlot("texture")->SetGpuImages("depthImage", m_depthInputViews);
	if (blurMaterialInstanceB.HasValue())
		blurMaterialInstanceB->GetSlot("texture")->SetGpuImages("depthImage", m_depthInputViews);
}

void HbaoPass::Create(const Vector2ui& size) {
	RenderTargetAttachmentInfo info{};
	info.format = Format::R16_SFLOAT;
	info.usage = GpuImageUsage::COMPUTE | GpuImageUsage::SAMPLED;
	info.name = "HBAO PreBlur";
	info.sampler = GpuImageSamplerDesc::CreateDefault();
	info.sampler.addressMode = GpuImageAddressMode::EDGE;
	unblurredHbaoTarget.Create(CalcualteTargetSize(size), info);

	info.name = "HBAO Blur 1";
	firstBlurTarget.Create(size, info);

	info.name = "HBAO RenderTarget";
	secondBlurTarget.Create(size, info);

	// Carga los materiales y crea los material instances.
	LoadMaterials();

	std::array<const GpuBuffer*, NUM_RESOURCES_IN_FLIGHT> buffers{};

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		m_cameraBuffers[i] = Engine::GetRenderer()->GetAllocator()->CreateBuffer(
			sizeof(glm::mat4) * 2 + sizeof(float), 0,
			GpuBufferUsage::UNIFORM_BUFFER,
			GpuSharedMemoryType::GPU_AND_CPU
		).GetPointer();

		m_cameraBuffers[i]->ResetCursor();
		m_cameraBuffers[i]->MapMemory();
		m_cameraBuffers[i]->Write(glm::mat4(1.0f));
		m_cameraBuffers[i]->Write(glm::mat4(1.0f));
		m_cameraBuffers[i]->Write(1.0f);
		m_cameraBuffers[i]->Unmap();

		buffers[i] = m_cameraBuffers[i].GetPointer();
	}

	hbaoMaterialInstance->GetSlot("camera")->SetUniformBuffers("camera", buffers);
	hbaoMaterialInstance->GetSlot("camera")->FlushUpdate();

	// Ruido
	const auto noiseTexture = Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/BlueNoise/bluenoise.json");
	const auto viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	hbaoMaterialInstance->GetSlot("texture")->SetGpuImage("noiseImage", noiseTexture.GetAsset()->GetGpuImage()->GetView(viewConfig));
	hbaoMaterialInstance->GetSlot("texture")->FlushUpdate();

	// Blur
	SetupBlurChain();

	IPostProcessPass::Create(size);

	UpdateMaterialInstance();
}

void HbaoPass::Resize(const Vector2ui& size) {
	unblurredHbaoTarget.Resize(CalcualteTargetSize(size));
	firstBlurTarget.Resize(size);
	secondBlurTarget.Resize(size);

	IPostProcessPass::Resize(size);

	SetupBlurChain();
}

Vector2ui HbaoPass::CalcualteTargetSize(Vector2ui nativeRes) const {
	return Vector2ui(
		static_cast<unsigned int>(glm::ceil(nativeRes.x / (1.0f / m_renderSizeRatio))),
		static_cast<unsigned int>(glm::ceil(nativeRes.y / (1.0f / m_renderSizeRatio)))
	);
}

void HbaoPass::LoadMaterials() {
	hbaoMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PostProcess/HBAO/hbao.json");
	hbaoMaterialInstance = hbaoMaterial->CreateInstance().GetPointer();

	blurMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PostProcess/HBAO/blur.json");
	blurMaterialInstanceA = blurMaterial->CreateInstance().GetPointer();
	blurMaterialInstanceB = blurMaterial->CreateInstance().GetPointer();

	postProcessingMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PostProcess/HBAO/resolve.json");
	postProcessingMaterialInstance = postProcessingMaterial->CreateInstance().GetPointer();
}

void HbaoPass::SetupBlurChain() {
	// Config de view.
	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	const GpuImageViewConfig writeConfig = GpuImageViewConfig::CreateStorage_Default();

	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> hbaoImages{};

	// Primer blur: hbao -> firstTarget
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		hbaoImages[i] = unblurredHbaoTarget.GetTargetImage(i)->GetView(viewConfig);
	blurMaterialInstanceA->GetSlot("texture")->SetGpuImages("unblurredHbao", hbaoImages);

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		hbaoImages[i] = firstBlurTarget.GetTargetImage(i)->GetView(writeConfig);
	blurMaterialInstanceA->GetSlot("texture")->SetStorageImages("finalImage", hbaoImages);

	// Segundo blur: firstTarget -> secondTarget
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		hbaoImages[i] = firstBlurTarget.GetTargetImage(i)->GetView(viewConfig);
	blurMaterialInstanceB->GetSlot("texture")->SetGpuImages("unblurredHbao", hbaoImages);

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		hbaoImages[i] = secondBlurTarget.GetTargetImage(i)->GetView(writeConfig);
	blurMaterialInstanceB->GetSlot("texture")->SetStorageImages("finalImage", hbaoImages);

	// Resolve: secondTarget -> finalTarget
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		hbaoImages[i] = secondBlurTarget.GetTargetImage(i)->GetView(viewConfig);

	postProcessingMaterialInstance->GetSlot("texture")->SetGpuImages("hbaoImage", hbaoImages);
}

void HbaoPass::UpdateCamera(const glm::mat4& inverseProjection, const glm::mat4& view, float nearPlane) {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	m_cameraBuffers[resourceIndex]->ResetCursor();
	m_cameraBuffers[resourceIndex]->MapMemory();
	m_cameraBuffers[resourceIndex]->Write(inverseProjection);
	m_cameraBuffers[resourceIndex]->Write(view);
	m_cameraBuffers[resourceIndex]->Write(nearPlane);
	m_cameraBuffers[resourceIndex]->Unmap();
}

void HbaoPass::UpdateMaterialInstance() {
	hbaoMaterialInstance->GetSlot("texture")->FlushUpdate();
	hbaoMaterialInstance->GetSlot("camera")->FlushUpdate();

	blurMaterialInstanceA->GetSlot("texture")->FlushUpdate();
	blurMaterialInstanceB->GetSlot("texture")->FlushUpdate();

	IPostProcessPass::UpdateMaterialInstance();
}

void HbaoPass::SetInput(std::span<GpuImage*, NUM_RESOURCES_IN_FLIGHT> images, const GpuImageViewConfig& viewConfig) {
	IPostProcessPass::SetInput(images, viewConfig);

	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> hbaoImages{};
	const GpuImageViewConfig writeConfig = GpuImageViewConfig::CreateStorage_Default();

	// Scene para el pase HBAO
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		hbaoImages[i] = inputViews[i];
	hbaoMaterialInstance->GetSlot("texture")->SetGpuImages("sceneImage", hbaoImages);

	// Target
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		hbaoImages[i] = unblurredHbaoTarget.GetTargetImage(i)->GetView(writeConfig);
	hbaoMaterialInstance->GetSlot("texture")->SetStorageImages("finalImage", hbaoImages);
}

void HbaoPass::Execute(ICommandList* computeCmdList) {
	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	computeCmdList->StartDebugSection("HBAO", Color::Purple);

#pragma region HBAO

	computeCmdList->StartDebugSection("HBAO", Color::Purple);

	GpuImageRange depthImageRange{};
	depthImageRange.channel = SampledChannel::DEPTH;

	computeCmdList->SetGpuImageBarrier(
		inputImages[resourceIndex],
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	computeCmdList->SetGpuImageBarrier(
		m_depthInputs[resourceIndex],
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ),
		depthImageRange);

	computeCmdList->SetGpuImageBarrier(
		m_normalInputs[resourceIndex],
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	computeCmdList->SetGpuImageBarrier(
		unblurredHbaoTarget.GetTargetImage(resourceIndex),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	computeCmdList->BindMaterial(*hbaoMaterial);
	computeCmdList->BindMaterialSlot(*hbaoMaterialInstance->GetSlot("texture"));
	computeCmdList->BindMaterialSlot(*hbaoMaterialInstance->GetSlot("camera"));

	Vector3ui dispatchRes = {
		static_cast<UIndex32>(glm::ceil(static_cast<float>(unblurredHbaoTarget.GetSize().x) / 8.0f)),
		static_cast<UIndex32>(glm::ceil(static_cast<float>(unblurredHbaoTarget.GetSize().y) / 8.0f)),
		1
	};

	computeCmdList->DispatchCompute(dispatchRes);
	computeCmdList->EndDebugSection();

#pragma endregion

	// Blur: native resolution
	dispatchRes = {
		static_cast<UIndex32>(glm::ceil(static_cast<float>(firstBlurTarget.GetSize().x) / 8.0f)),
		static_cast<UIndex32>(glm::ceil(static_cast<float>(firstBlurTarget.GetSize().y) / 8.0f)),
		1
	};

	// Primer blur
	computeCmdList->StartDebugSection("First Blur", Color::Purple);

	computeCmdList->SetGpuImageBarrier(
		unblurredHbaoTarget.GetTargetImage(resourceIndex),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	computeCmdList->SetGpuImageBarrier(
		firstBlurTarget.GetTargetImage(resourceIndex),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	computeCmdList->BindMaterial(*blurMaterial);
	computeCmdList->BindMaterialSlot(*blurMaterialInstanceA->GetSlot("texture"));
	computeCmdList->PushMaterialConstants<int>("direction", 0);
	computeCmdList->DispatchCompute(dispatchRes);

	computeCmdList->EndDebugSection();

	// Segundo blur
	computeCmdList->StartDebugSection("Second Blur", Color::Purple);

	computeCmdList->SetGpuImageBarrier(
		firstBlurTarget.GetTargetImage(resourceIndex),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	computeCmdList->SetGpuImageBarrier(
		secondBlurTarget.GetTargetImage(resourceIndex),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	computeCmdList->BindMaterial(*blurMaterial);
	computeCmdList->BindMaterialSlot(*blurMaterialInstanceB->GetSlot("texture"));
	computeCmdList->PushMaterialConstants<int>("direction", 1);
	computeCmdList->DispatchCompute(dispatchRes);

	computeCmdList->EndDebugSection();

	// Resolve
	computeCmdList->StartDebugSection("Resolve", Color::Purple);

	computeCmdList->SetGpuImageBarrier(
		secondBlurTarget.GetTargetImage(resourceIndex),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	computeCmdList->SetGpuImageBarrier(
		resolveRenderTarget.GetTargetImage(resourceIndex),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	computeCmdList->BindMaterial(*postProcessingMaterial);
	computeCmdList->BindMaterialSlot(*postProcessingMaterialInstance->GetSlot("texture"));
	computeCmdList->DispatchCompute(dispatchRes);

	computeCmdList->EndDebugSection();

	computeCmdList->EndDebugSection();
}
