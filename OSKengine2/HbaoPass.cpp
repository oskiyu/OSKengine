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

void HbaoPass::SetNormalsInput(GpuImage* image) {
	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);

	m_normalInput = image;
	m_normalInputView = image->GetView(viewConfig);

	if (m_hbaoMaterialInstance.HasValue()) {
		m_hbaoMaterialInstance->GetSlot("texture")->SetGpuImage("normalImage", m_normalInputView);
	}
}

void HbaoPass::SetDepthInput(GpuImage* image) {
	GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	viewConfig.channel = SampledChannel::DEPTH;

	m_depthInput = image;
	m_depthInputView = image->GetView(viewConfig);

	if (m_hbaoMaterialInstance.HasValue()) {
		m_hbaoMaterialInstance->GetSlot("texture")->SetGpuImage("depthImage", m_depthInputView);
	}

	if (m_blurMaterialInstanceA.HasValue()) {
		m_blurMaterialInstanceA->GetSlot("texture")->SetGpuImage("depthImage", m_depthInputView);
	}

	if (m_blurMaterialInstanceB.HasValue()) {
		m_blurMaterialInstanceB->GetSlot("texture")->SetGpuImage("depthImage", m_depthInputView);
	}
}

void HbaoPass::Create(const Vector2ui& size) {
	RenderTargetAttachmentInfo info{};
	info.format = Format::R16_SFLOAT;
	info.usage = GpuImageUsage::COMPUTE | GpuImageUsage::SAMPLED;
	info.name = "HBAO PreBlur";
	info.sampler = GpuImageSamplerDesc::CreateDefault();
	info.sampler.addressMode = GpuImageAddressMode::EDGE;
	m_unblurredHbaoTarget.Create(CalcualteTargetSize(size), info);

	info.name = "HBAO Blur 1";
	m_firstBlurTarget.Create(size, info);

	info.name = "HBAO RenderTarget";
	m_secondBlurTarget.Create(size, info);

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

	m_hbaoMaterialInstance->GetSlot("camera")->SetUniformBuffers("camera", buffers);
	m_hbaoMaterialInstance->GetSlot("camera")->FlushUpdate();

	// Ruido
	const auto noiseTexture = Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/BlueNoise/bluenoise.json");
	const auto viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	m_hbaoMaterialInstance->GetSlot("texture")->SetGpuImage("noiseImage", noiseTexture.GetAsset()->GetGpuImage()->GetView(viewConfig));
	m_hbaoMaterialInstance->GetSlot("texture")->FlushUpdate();

	// Blur
	SetupBlurChain();

	IPostProcessPass::Create(size);

	UpdateMaterialInstance();
}

void HbaoPass::Resize(const Vector2ui& size) {
	m_unblurredHbaoTarget.Resize(CalcualteTargetSize(size));
	m_firstBlurTarget.Resize(size);
	m_secondBlurTarget.Resize(size);

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
	m_hbaoMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PostProcess/HBAO/hbao.json");
	m_hbaoMaterialInstance = m_hbaoMaterial->CreateInstance().GetPointer();

	m_blurMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PostProcess/HBAO/blur.json");
	m_blurMaterialInstanceA = m_blurMaterial->CreateInstance().GetPointer();
	m_blurMaterialInstanceB = m_blurMaterial->CreateInstance().GetPointer();

	postProcessingMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PostProcess/HBAO/resolve.json");
	postProcessingMaterialInstance = postProcessingMaterial->CreateInstance().GetPointer();
}

void HbaoPass::SetupBlurChain() {
	// Config de view.
	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	const GpuImageViewConfig writeConfig = GpuImageViewConfig::CreateStorage_Default();


	// Primer blur: hbao -> firstTarget
	m_blurMaterialInstanceA->GetSlot("texture")->SetGpuImage("unblurredHbao", 
		m_unblurredHbaoTarget.GetTargetImage()->GetView(viewConfig));

	m_blurMaterialInstanceA->GetSlot("texture")->SetStorageImage("finalImage",
		m_firstBlurTarget.GetTargetImage()->GetView(writeConfig));

	// Segundo blur: firstTarget -> secondTarget
	m_blurMaterialInstanceB->GetSlot("texture")->SetGpuImage("unblurredHbao",
		m_firstBlurTarget.GetTargetImage()->GetView(viewConfig));

	m_blurMaterialInstanceB->GetSlot("texture")->SetStorageImage("finalImage",
		m_secondBlurTarget.GetTargetImage()->GetView(writeConfig));

	// Resolve: secondTarget -> finalTarget
	postProcessingMaterialInstance->GetSlot("texture")->SetGpuImage("hbaoImage",
		m_secondBlurTarget.GetTargetImage()->GetView(viewConfig));
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
	m_hbaoMaterialInstance->GetSlot("texture")->FlushUpdate();
	m_hbaoMaterialInstance->GetSlot("camera")->FlushUpdate();

	m_blurMaterialInstanceA->GetSlot("texture")->FlushUpdate();
	m_blurMaterialInstanceB->GetSlot("texture")->FlushUpdate();

	IPostProcessPass::UpdateMaterialInstance();
}

void HbaoPass::SetInput(GpuImage* image, const GpuImageViewConfig& viewConfig) {
	IPostProcessPass::SetInput(image, viewConfig);

	const GpuImageViewConfig writeConfig = GpuImageViewConfig::CreateStorage_Default();

	// Scene para el pase HBAO
	m_hbaoMaterialInstance->GetSlot("texture")->SetGpuImage("sceneImage", inputView);

	// Target
	m_hbaoMaterialInstance->GetSlot("texture")->SetStorageImage("finalImage",
		m_unblurredHbaoTarget.GetTargetImage()->GetView(writeConfig));
}

void HbaoPass::Execute(ICommandList* computeCmdList) {
	computeCmdList->StartDebugSection("HBAO", Color::Purple);

#pragma region HBAO

	computeCmdList->StartDebugSection("HBAO", Color::Purple);

	GpuImageRange depthImageRange{};
	depthImageRange.channel = SampledChannel::DEPTH;

	computeCmdList->SetGpuImageBarrier(
		inputImage,
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	computeCmdList->SetGpuImageBarrier(
		m_depthInput,
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ),
		depthImageRange);

	computeCmdList->SetGpuImageBarrier(
		m_normalInput,
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	computeCmdList->SetGpuImageBarrier(
		m_unblurredHbaoTarget.GetTargetImage(),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	computeCmdList->BindMaterial(*m_hbaoMaterial);
	computeCmdList->BindMaterialSlot(*m_hbaoMaterialInstance->GetSlot("texture"));
	computeCmdList->BindMaterialSlot(*m_hbaoMaterialInstance->GetSlot("camera"));

	Vector3ui dispatchRes = {
		static_cast<UIndex32>(glm::ceil(static_cast<float>(m_unblurredHbaoTarget.GetSize().x) / 8.0f)),
		static_cast<UIndex32>(glm::ceil(static_cast<float>(m_unblurredHbaoTarget.GetSize().y) / 8.0f)),
		1
	};

	computeCmdList->DispatchCompute(dispatchRes);
	computeCmdList->EndDebugSection();

#pragma endregion

	// Blur: native resolution
	dispatchRes = {
		static_cast<UIndex32>(glm::ceil(static_cast<float>(m_firstBlurTarget.GetSize().x) / 8.0f)),
		static_cast<UIndex32>(glm::ceil(static_cast<float>(m_firstBlurTarget.GetSize().y) / 8.0f)),
		1
	};

	// Primer blur
	computeCmdList->StartDebugSection("First Blur", Color::Purple);

	computeCmdList->SetGpuImageBarrier(
		m_unblurredHbaoTarget.GetTargetImage(),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	computeCmdList->SetGpuImageBarrier(
		m_firstBlurTarget.GetTargetImage(),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	computeCmdList->BindMaterial(*m_blurMaterial);
	computeCmdList->BindMaterialSlot(*m_blurMaterialInstanceA->GetSlot("texture"));
	computeCmdList->PushMaterialConstants<int>("direction", 0);
	computeCmdList->DispatchCompute(dispatchRes);

	computeCmdList->EndDebugSection();

	// Segundo blur
	computeCmdList->StartDebugSection("Second Blur", Color::Purple);

	computeCmdList->SetGpuImageBarrier(
		m_firstBlurTarget.GetTargetImage(),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	computeCmdList->SetGpuImageBarrier(
		m_secondBlurTarget.GetTargetImage(),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	computeCmdList->BindMaterial(*m_blurMaterial);
	computeCmdList->BindMaterialSlot(*m_blurMaterialInstanceB->GetSlot("texture"));
	computeCmdList->PushMaterialConstants<int>("direction", 1);
	computeCmdList->DispatchCompute(dispatchRes);

	computeCmdList->EndDebugSection();

	// Resolve
	computeCmdList->StartDebugSection("Resolve", Color::Purple);

	computeCmdList->SetGpuImageBarrier(
		m_secondBlurTarget.GetTargetImage(),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	computeCmdList->SetGpuImageBarrier(
		resolveRenderTarget.GetTargetImage(),
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
