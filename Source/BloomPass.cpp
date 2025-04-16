#include "BloomPass.h"

#include "NumericTypes.h"
#include "Vector2.hpp"

#include "OSKengine.h"
#include "IRenderer.h"

#include "MaterialSystem.h"
#include "Material.h"

#include "ICommandList.h"

#include "IGpuImage.h"
#include "GpuImageSamplerDesc.h"
#include "GpuImageLayout.h"
#include "CopyImageInfo.h"
#include "GpuImageViewConfig.h"
#include "Format.h"
#include "RenderTargetAttachmentInfo.h"
#include "GpuImageUsage.h"
#include "PostProcessExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

constexpr float BLOCK_SIZE = 8.0f;

void BloomPass::SetInput(GpuImage* inputImage) {
	m_inputImage = inputImage;
	SetupMaterialInstances();
}

void BloomPass::Create(const Vector2ui& size) {
	// Carga de materiales.
	m_downscaleMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial(DownscaleMaterial);
	m_upscaleMaterial   = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial(UpscaleMaterial);
	m_resolveMaterial   = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial(ResolveMaterial);

	// Final render target
	GpuImageSamplerDesc renderTargetSampler{};
	renderTargetSampler.addressMode = GpuImageAddressMode::EDGE;		// Para evitar que los bordes tengan leaks de bloom.
	renderTargetSampler.filteringType = GpuImageFilteringType::LINEAR;	// Para los tap que agrupan varios píxeles.
	renderTargetSampler.mipMapMode = GpuImageMipmapMode::CUSTOM;		// Cada nivel de bloom se ejecuta en un mipmap.
	renderTargetSampler.minMipLevel = 0;
	renderTargetSampler.maxMipLevel = maxNumPasses;						// Cada nivel de bloom se ejecuta en un mipmap.

	RenderTargetAttachmentInfo resolveInfo{};
	resolveInfo.format = Format::RGBA16_SFLOAT;
	resolveInfo.sampler = renderTargetSampler;
	resolveInfo.name = "Bloom Output";
	resolveInfo.usage = 
		GpuImageUsage::TRANSFER_DESTINATION | 
		GpuImageUsage::COMPUTE | 
		GpuImageUsage::SAMPLED;


	// Inicialización de instancias del material.
	GetOutput().Create(size, resolveInfo);
	m_resolveInstance = m_resolveMaterial->CreateInstance();

	for (UIndex32 pass = 0; pass < maxNumPasses; pass++) {
		m_downscalingMaterialInstances[pass] = m_downscaleMaterial->CreateInstance();
		m_upscalingMaterialInstances[pass] = m_upscaleMaterial->CreateInstance();
	}
}

void BloomPass::Resize(const Vector2ui& size) {
	GetOutput().Resize(size);
	SetupMaterialInstances();
}

void BloomPass::SetupMaterialInstances() {
	GpuImageViewConfig sourceViewConfig = GpuImageViewConfig::CreateSampled_MipLevelRanged(0, 0);
	GpuImageViewConfig destViewConfig = GpuImageViewConfig::CreateStorage_Default();

	const UIndex32 lowestLevel = GetNumPasses();

	for (UIndex32 pass = 0; pass < lowestLevel - 1; pass++) {

		sourceViewConfig.baseMipLevel = pass;
		sourceViewConfig.topMipLevel = pass;

		destViewConfig.baseMipLevel = pass + 1;
		destViewConfig.topMipLevel = pass + 1;

		const auto& sourceView = *GetOutput().GetTargetImage()->GetView(sourceViewConfig);
		const auto& targetView = *GetOutput().GetTargetImage()->GetView(destViewConfig);

		m_downscalingMaterialInstances[pass]->GetSlot("texture")->SetGpuImage("inputImg", sourceView, GpuImageSamplerDesc::CreateDefault_NoMipMap());
		m_downscalingMaterialInstances[pass]->GetSlot("texture")->SetStorageImage("outputImg", targetView);
		m_downscalingMaterialInstances[pass]->GetSlot("texture")->FlushUpdate();
	}

	for (UIndex32 pass = lowestLevel - 2; pass > 0; pass--) {

		sourceViewConfig.baseMipLevel = pass + 1;
		sourceViewConfig.topMipLevel = pass + 1;

		destViewConfig.baseMipLevel = pass;
		destViewConfig.topMipLevel = pass;

		const auto& sourceView = GetOutput().GetTargetImage()->GetView(sourceViewConfig);
		const auto& targetView = GetOutput().GetTargetImage()->GetView(destViewConfig);

		m_upscalingMaterialInstances[pass]->GetSlot("texture")->SetGpuImage("inputImg", *sourceView, GpuImageSamplerDesc::CreateDefault_NoMipMap());
		m_upscalingMaterialInstances[pass]->GetSlot("texture")->SetStorageImage("outputImg", *targetView);
		m_upscalingMaterialInstances[pass]->GetSlot("texture")->FlushUpdate();
	}

	// Resolve
	sourceViewConfig.baseMipLevel = 1;
	sourceViewConfig.topMipLevel = 1;

	destViewConfig.baseMipLevel = 0;
	destViewConfig.topMipLevel = 0;

	const auto& sourceView = *GetOutput().GetTargetImage()->GetView(sourceViewConfig);
	const auto& sceneView  = *m_inputImage->GetView(GpuImageViewConfig::CreateSampled_SingleMipLevel(0));
	const auto& targetView = *GetOutput().GetTargetImage()->GetView(destViewConfig);

	m_resolveInstance->GetSlot("texture")->SetGpuImage("bloomImg", sourceView, GpuImageSamplerDesc::CreateDefault_NoMipMap());
	m_resolveInstance->GetSlot("texture")->SetGpuImage("sceneImg", sceneView, GpuImageSamplerDesc::CreateDefault_NoMipMap());
	m_resolveInstance->GetSlot("texture")->SetStorageImage("outputImg", targetView);
	m_resolveInstance->GetSlot("texture")->FlushUpdate();
}

void BloomPass::ExecuteSinglePass(ICommandList* computeCmdList, UIndex32 sourceMipLevel, UIndex32 destMipLevel) {
	const Vector2f sourceResolution = GetOutput().GetTargetImage()->GetMipLevelSize2D(sourceMipLevel).ToVector2f();
	const Vector2f destResolution = GetOutput().GetTargetImage()->GetMipLevelSize2D(destMipLevel).ToVector2f();

	const auto maxResolution = Vector2f(
		glm::max(sourceResolution.x, destResolution.x),
		glm::max(sourceResolution.y, destResolution.y)
	);

	const Vector2ui dispatchRes = {
		static_cast<unsigned int>(glm::ceil(maxResolution.x / BLOCK_SIZE)),
		static_cast<unsigned int>(glm::ceil(maxResolution.y / BLOCK_SIZE)),
	};

	computeCmdList->DispatchCompute({ dispatchRes.x, dispatchRes.y, 1 });
}

void BloomPass::DownscaleBloom(GRAPHICS::ICommandList* computeCmdList) {
	computeCmdList->StartDebugSection("Downscale", Color::Purple);

	computeCmdList->BindMaterial(*m_downscaleMaterial);
	for (UIndex32 i = 0; i < GetNumPasses() - 1; i++) {
		computeCmdList->BindMaterialSlot(*m_downscalingMaterialInstances[i]->GetSlot("texture"));

		if (i == 0)
			computeCmdList->PushMaterialConstants("info", 1);
		else
			computeCmdList->PushMaterialConstants("info", 0);
		
		const UIndex32 sourceMipLevel = i;
		const UIndex32 destinationMipLevel = sourceMipLevel + 1;

		// Nivel anterior debe ser SAMPLED.
		computeCmdList->SetGpuImageBarrier(
			GetOutput().GetTargetImage(),
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ),
			{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = sourceMipLevel, .numMipLevels = 1 });

		// Nivel siguiente será escrito.
		// Al no haber sido usado previamente en este frame,
		// layout inicial es UNDEFINED y barrier es NONE.
		computeCmdList->SetGpuImageBarrier(
			GetOutput().GetTargetImage(),
			GpuImageLayout::UNDEFINED,
			GpuImageLayout::GENERAL,
			GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
			GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE),
			{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = destinationMipLevel, .numMipLevels = 1 });

		ExecuteSinglePass(computeCmdList, sourceMipLevel, destinationMipLevel);
	}

	computeCmdList->EndDebugSection();
}

void BloomPass::UpscaleBloom(GRAPHICS::ICommandList* computeCmdList) {
	computeCmdList->StartDebugSection("Upscale", Color::Purple);

	computeCmdList->BindMaterial(*m_upscaleMaterial);

	for (UIndex32 i = GetNumPasses() - 2; i > 0; i--) {
		computeCmdList->BindMaterialSlot(*m_upscalingMaterialInstances[i]->GetSlot("texture"));

		const UIndex32 sourceMipLevel = i + 1;
		const UIndex32 destinationMipLevel = i;

		// Nivel anterior debe ser SAMPLED.
		computeCmdList->SetGpuImageBarrier(
			GetOutput().GetTargetImage(),
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ),
			{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = sourceMipLevel, .numMipLevels = 1 });

		// Nivel siguiente será escrito (y leído).
		computeCmdList->SetGpuImageBarrier(
			GetOutput().GetTargetImage(),
			GpuImageLayout::GENERAL,
			GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ | GpuAccessStage::SHADER_WRITE),
			{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = destinationMipLevel, .numMipLevels = 1 });

		ExecuteSinglePass(computeCmdList, sourceMipLevel, destinationMipLevel);
	}

	computeCmdList->EndDebugSection();
}

void BloomPass::InitialCopy(ICommandList* computeCmdList) {
	computeCmdList->AddDebugMarker("Source Copy", Color::Yellow);

	// La imagen de la escena se copia a la cadena de mipmaps:
	// debe estar en transfer source.
	computeCmdList->SetGpuImageBarrier(
		m_inputImage,
		GpuImageLayout::TRANSFER_SOURCE,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_READ));

	// La imagen de destino (mip 0) debe ser transfer dest. para efectuar la copia.
	// Al no haber sido usada previamente, el layout original es UNDEFINED y
	// barrier info es NONE.
	computeCmdList->SetGpuImageBarrier(
		GetOutput().GetTargetImage(),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE),
		{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = 1 });

	const Vector2ui imgSize = m_inputImage->GetSize2D();
	CopyImageInfo copyInfo = CopyImageInfo::CreateDefault2D(imgSize);
	copyInfo.destinationMipLevel = 0;

	computeCmdList->RawCopyImageToImage(
		*m_inputImage,
		GetOutput().GetTargetImage(),
		copyInfo);
}

void BloomPass::Resolve(ICommandList* computeCmdList) {
	computeCmdList->StartDebugSection("Resolve", Color::Purple);

	// Source = nivel 1
	computeCmdList->SetGpuImageBarrier(
		GetOutput().GetTargetImage(),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ),
		{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = 1, .numMipLevels = 1 });

	// Dest = nivel 0
	computeCmdList->SetGpuImageBarrier(
		GetOutput().GetTargetImage(),
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ | GpuAccessStage::SHADER_WRITE),
		{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = 1 });

	// La imagen de la escena se vuelve a usar.
	computeCmdList->SetGpuImageBarrier(
		m_inputImage,
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	const Vector2ui dispatchRes = {
		static_cast<UIndex32>(glm::ceil(GetOutput().GetSize().x / BLOCK_SIZE)),
		static_cast<UIndex32>(glm::ceil(GetOutput().GetSize().y / BLOCK_SIZE))
	};

	computeCmdList->BindMaterial(*m_resolveMaterial);
	computeCmdList->BindMaterialSlot(*m_resolveInstance->GetSlot("texture"));
	computeCmdList->DispatchCompute({ dispatchRes.x, dispatchRes.y, 1 });

	computeCmdList->EndDebugSection();
}

void BloomPass::Execute(ICommandList* computeCmdList) {
	OSK_ASSERT(
		m_inputImage != nullptr,
		PostProcessInputNotSetException("No se ha establecido la entrada de color."));

	computeCmdList->StartDebugSection("Bloom", Color::Purple);

	InitialCopy(computeCmdList);
	DownscaleBloom(computeCmdList);
	UpscaleBloom(computeCmdList);
	Resolve(computeCmdList);

	computeCmdList->EndDebugSection();
}

UIndex32 BloomPass::GetNumPasses() const {
	return glm::min(
		GetOutput().GetTargetImage()->GetMipLevels(),
		maxNumPasses
	);
}
