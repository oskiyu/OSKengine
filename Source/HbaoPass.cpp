#include "HbaoPass.h"

#include "ICommandList.h"

#include "OSKengine.h"
#include "MaterialSystem.h"
#include "Material.h"

#include "AssetManager.h"
#include "Texture.h"

#include "IGpuImage.h"
#include "IGpuImageView.h"

#include "GpuImageLayout.h"

#include "PostProcessExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


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

	info.name = "HBAO Blur 2";
	m_secondBlurTarget.Create(size, info);

	info.name = "HBAO RenderTarget";
	info.format = Format::RGBA16_SFLOAT;
	info.usage |= GpuImageUsage::TRANSFER_SOURCE;
	GetOutput().Create(size, info);

	// Carga los materiales y crea los material instances.
	LoadMaterials();

	for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		m_cameraBuffers[i] = Engine::GetRenderer()->GetAllocator()->CreateBuffer(
			sizeof(glm::mat4) * 2 + sizeof(float), GPU_MEMORY_NO_ALIGNMENT,
			GpuBufferUsage::UNIFORM_BUFFER,
			GpuSharedMemoryType::GPU_AND_CPU,
			GpuQueueType::MAIN
		).GetPointer();

		m_cameraBuffers[i]->ResetCursor();
		m_cameraBuffers[i]->MapMemory();
		m_cameraBuffers[i]->Write(glm::mat4(1.0f));
		m_cameraBuffers[i]->Write(glm::mat4(1.0f));
		m_cameraBuffers[i]->Write(1.0f);
		m_cameraBuffers[i]->Unmap();

		m_hbaoMaterialInstances[i]->GetSlot("camera")->SetUniformBuffer("camera", m_cameraBuffers[i].GetValue());
		m_hbaoMaterialInstances[i]->GetSlot("camera")->FlushUpdate();
	}


	// Ruido
	const auto noiseTexture = Engine::GetAssetManager()->Load<ASSETS::Texture>("Resources/Assets/BlueNoise/bluenoise.json");
	const auto viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	const auto writeConfig = GpuImageViewConfig::CreateStorage_Default();

	for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		m_hbaoMaterialInstances[i]->GetSlot("texture")->SetGpuImage("noiseImage", *noiseTexture.GetAsset()->GetGpuImage()->GetView(viewConfig));
		m_hbaoMaterialInstances[i]->GetSlot("texture")->SetStorageImage("finalImage", *m_unblurredHbaoTarget.GetTargetImage()->GetView(writeConfig));
		m_hbaoMaterialInstances[i]->GetSlot("texture")->FlushUpdate();
	}

	// Blur
	SetupBlurChain();

	// Resolución.
	const GpuImageViewConfig outputViewConfig = GpuImageViewConfig::CreateTarget_Color();
	m_resolveMaterialInstance->GetSlot("texture")->SetStorageImage("finalImage", *GetOutput().GetTargetImage()->GetView(outputViewConfig));
	m_resolveMaterialInstance->GetSlot("texture")->SetGpuImage("hbaoImage", *m_secondBlurTarget.GetTargetImage()->GetView(viewConfig));
	m_resolveMaterialInstance->GetSlot("texture")->FlushUpdate(); // Está incompleto, falta "sceneImage".
}

void HbaoPass::SetColorInput(GpuImage* image) {
	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	const IGpuImageView& view = *image->GetView(viewConfig);

	m_resolveMaterialInstance->GetSlot("texture")->SetGpuImage("sceneImage", view);
	m_resolveMaterialInstance->GetSlot("texture")->FlushUpdate();

	m_colorInput = image;

	// TODO: eliminar
	for (auto& mInstance : m_hbaoMaterialInstances) {
		mInstance->GetSlot("texture")->SetGpuImage("sceneImage", view);
		mInstance->GetSlot("texture")->FlushUpdate();
	}

}

void HbaoPass::SetNormalsInput(GpuImage* image) {
	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);

	m_normalInput = image;

	for (auto& mInstance : m_hbaoMaterialInstances) {
		mInstance->GetSlot("texture")->SetGpuImage("normalImage", *image->GetView(viewConfig));
		mInstance->GetSlot("texture")->FlushUpdate();
	}
}

void HbaoPass::SetDepthInput(GpuImage* image) {
	GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	viewConfig.channel = SampledChannel::DEPTH;

	m_depthInput = image;

	for (auto& mInstance : m_hbaoMaterialInstances) {
		mInstance->GetSlot("texture")->SetGpuImage("depthImage", *image->GetView(viewConfig));
		mInstance->GetSlot("texture")->FlushUpdate();
	}

	m_blurMaterialInstanceA->GetSlot("texture")->SetGpuImage("depthImage", *image->GetView(viewConfig));
	m_blurMaterialInstanceA->GetSlot("texture")->FlushUpdate();
	m_blurMaterialInstanceB->GetSlot("texture")->SetGpuImage("depthImage", *image->GetView(viewConfig));
	m_blurMaterialInstanceB->GetSlot("texture")->FlushUpdate();
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
	m_hbaoMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial(HbaoMaterial);
	for (auto& mInstance : m_hbaoMaterialInstances) {
		mInstance = m_hbaoMaterial->CreateInstance().GetPointer();
	}

	m_blurMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial(BlurMaterial);
	m_blurMaterialInstanceA = m_blurMaterial->CreateInstance().GetPointer();
	m_blurMaterialInstanceB = m_blurMaterial->CreateInstance().GetPointer();

	m_resolveMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial(ResolveMaterial);
	m_resolveMaterialInstance = m_resolveMaterial->CreateInstance().GetPointer();
}

void HbaoPass::SetupBlurChain() {
	// Config de view.
	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_SingleMipLevel(0);
	const GpuImageViewConfig writeConfig = GpuImageViewConfig::CreateStorage_Default();

	// Actualizar primer target.
	for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		m_hbaoMaterialInstances[i]->GetSlot("texture")->SetStorageImage("finalImage", *m_unblurredHbaoTarget.GetTargetImage()->GetView(writeConfig));
		m_hbaoMaterialInstances[i]->GetSlot("texture")->FlushUpdate();
	}

	// Primer blur: hbao -> firstTarget
	{
		MaterialInstance* mInstance = m_blurMaterialInstanceA.GetPointer();
		IMaterialSlot* mSlot = mInstance->GetSlot("texture");

		mSlot->SetGpuImage("unblurredHbao", *m_unblurredHbaoTarget.GetTargetImage()->GetView(viewConfig));
		mSlot->SetStorageImage("finalImage", *m_firstBlurTarget.GetTargetImage()->GetView(writeConfig));
		mSlot->FlushUpdate();
	}

	// Segundo blur: firstTarget -> secondTarget
	{
		MaterialInstance* mInstance = m_blurMaterialInstanceB.GetPointer();
		IMaterialSlot* mSlot = mInstance->GetSlot("texture");

		mSlot->SetGpuImage("unblurredHbao", *m_firstBlurTarget.GetTargetImage()->GetView(viewConfig));
		mSlot->SetStorageImage("finalImage", *m_secondBlurTarget.GetTargetImage()->GetView(writeConfig));
		mSlot->FlushUpdate();

	}

	// Resolve: secondTarget -> finalTarget
	{
		MaterialInstance* mInstance = m_resolveMaterialInstance.GetPointer();
		IMaterialSlot* mSlot = mInstance->GetSlot("texture");

		const GpuImageViewConfig outputViewConfig = GpuImageViewConfig::CreateTarget_Color();

		mSlot->SetStorageImage("finalImage", *GetOutput().GetTargetImage()->GetView(outputViewConfig));
		mSlot->SetGpuImage("hbaoImage", *m_secondBlurTarget.GetTargetImage()->GetView(viewConfig));
		mSlot->FlushUpdate();
	}
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

void HbaoPass::Execute(ICommandList* computeCmdList) {
	OSK_ASSERT(
		m_colorInput != nullptr,
		PostProcessInputNotSetException("No se ha establecido la entrada de color."));
	OSK_ASSERT(
		m_depthInput != nullptr,
		PostProcessInputNotSetException("No se ha establecido la entrada de profundidad."));
	OSK_ASSERT(
		m_normalInput != nullptr,
		PostProcessInputNotSetException("No se ha establecido la entrada de normales."));

	const UIndex32 resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	computeCmdList->StartDebugSection("HBAO", Color::Purple);

#pragma region HBAO

	computeCmdList->StartDebugSection("HBAO", Color::Purple);

	GpuImageRange depthImageRange{};
	depthImageRange.channel = SampledChannel::DEPTH;

	computeCmdList->SetGpuImageBarrier(
		m_colorInput,
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
	computeCmdList->BindMaterialSlot(*m_hbaoMaterialInstances[resourceIndex]->GetSlot("texture"));
	computeCmdList->BindMaterialSlot(*m_hbaoMaterialInstances[resourceIndex]->GetSlot("camera"));

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
		GetOutput().GetTargetImage(),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	computeCmdList->BindMaterial(*m_resolveMaterial);
	computeCmdList->BindMaterialSlot(*m_resolveMaterialInstance->GetSlot("texture"));
	computeCmdList->DispatchCompute(dispatchRes);

	computeCmdList->EndDebugSection();

	computeCmdList->EndDebugSection();
}
