#include "BloomPass.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "MaterialSystem.h"
#include "Material.h"
#include "IGpuImage.h"
#include "GpuImageDimensions.h"
#include "GpuMemoryTypes.h"
#include "GpuImageSamplerDesc.h"
#include "GpuImageLayout.h"
#include "ICommandList.h"
#include "Viewport.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

using namespace OSK;
using namespace OSK::GRAPHICS;

constexpr float BLOCK_SIZE = 8.0f;

void BloomPass::Create(const Vector2ui& size) {
	// Carga de materiales.
	downscaleMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PostProcess/Bloom/downscale.json");
	upscaleMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PostProcess/Bloom/upscale.json");
	resolveMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PostProcess/Bloom/final.json");

	// Final render target
	GpuImageSamplerDesc renderTargetSampler{};
	renderTargetSampler.addressMode = GpuImageAddressMode::EDGE;		// Para evitar que los bordes tengan leaks de bloom.
	renderTargetSampler.filteringType = GpuImageFilteringType::LIENAR;	// Para los tap que agrupan varios píxeles.
	renderTargetSampler.mipMapMode = GpuImageMipmapMode::CUSTOM;			// Cada nivel de bloom se ejecuta en un mipmap.
	renderTargetSampler.minMipLevel = 0;
	renderTargetSampler.maxMipLevel = maxNumPasses;

	RenderTargetAttachmentInfo resolveInfo{};
	resolveInfo.format = Format::RGBA16_SFLOAT;
	resolveInfo.sampler = renderTargetSampler;
	resolveInfo.name = "Bloom Output";
	resolveInfo.usage = 
		GpuImageUsage::TRANSFER_DESTINATION | 
		GpuImageUsage::COMPUTE | 
		GpuImageUsage::SAMPLED;


	// Inicialización de instancias del material.
	resolveRenderTarget.Create(size, resolveInfo);
	resolveInstance = resolveMaterial->CreateInstance().GetPointer();

	for (TIndex pass = 0; pass < maxNumPasses; pass++) {
		downscalingMaterialInstance[pass] = downscaleMaterial->CreateInstance().GetPointer();
		upscalingMaterialInstance[pass] = upscaleMaterial->CreateInstance().GetPointer();
	}
}

void BloomPass::Resize(const Vector2ui& size) {
	resolveRenderTarget.Resize(size);
}

void BloomPass::SetupMaterialInstances() {
	const IGpuImageView* sourceImages[NUM_RESOURCES_IN_FLIGHT]{};
	const IGpuImageView* targetImages[NUM_RESOURCES_IN_FLIGHT]{};

	GpuImageViewConfig sourceViewConfig = GpuImageViewConfig::CreateSampled_MipLevelRanged(0, 0);
	GpuImageViewConfig destViewConfig = GpuImageViewConfig::CreateStorage_Default();

	const TIndex lowestLevel = GetNumPasses();

	for (TIndex pass = 0; pass < lowestLevel - 1; pass++) {

		sourceViewConfig.baseMipLevel = pass;
		sourceViewConfig.topMipLevel = pass;

		destViewConfig.baseMipLevel = pass + 1;
		destViewConfig.topMipLevel = pass + 1;

		for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
			sourceImages[i] = resolveRenderTarget.GetTargetImage(i)->GetView(sourceViewConfig);
			targetImages[i] = resolveRenderTarget.GetTargetImage(i)->GetView(destViewConfig);
		}

		downscalingMaterialInstance[pass]->GetSlot("texture")->SetGpuImages("inputImg", sourceImages);
		downscalingMaterialInstance[pass]->GetSlot("texture")->SetStorageImages("outputImg", targetImages);
		downscalingMaterialInstance[pass]->GetSlot("texture")->FlushUpdate();
	}

	for (TIndex pass = lowestLevel - 2; pass > 0; pass--) {

		sourceViewConfig.baseMipLevel = pass + 1;
		sourceViewConfig.topMipLevel = pass + 1;

		destViewConfig.baseMipLevel = pass;
		destViewConfig.topMipLevel = pass;

		for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
			targetImages[i] = resolveRenderTarget.GetTargetImage(i)->GetView(destViewConfig);
			sourceImages[i] = resolveRenderTarget.GetTargetImage(i)->GetView(sourceViewConfig);
		}

		upscalingMaterialInstance[pass]->GetSlot("texture")->SetGpuImages("inputImg", sourceImages);
		upscalingMaterialInstance[pass]->GetSlot("texture")->SetStorageImages("outputImg", targetImages);
		upscalingMaterialInstance[pass]->GetSlot("texture")->FlushUpdate();
	}

	// Resolve
	sourceViewConfig.baseMipLevel = 1;
	sourceViewConfig.topMipLevel = 1;

	destViewConfig.baseMipLevel = 0;
	destViewConfig.topMipLevel = 0;

	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		sourceImages[i] = resolveRenderTarget.GetTargetImage(i)->GetView(sourceViewConfig);

	const IGpuImageView* sceneImages[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		sceneImages[i] = inputImages[i]->GetView(destViewConfig);

	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		targetImages[i] = resolveRenderTarget.GetTargetImage(i)->GetView(destViewConfig);

	resolveInstance->GetSlot("texture")->SetGpuImages("bloomImg", sourceImages);
	resolveInstance->GetSlot("texture")->SetGpuImages("sceneImg", sceneImages);
	resolveInstance->GetSlot("texture")->SetStorageImages("outputImg", targetImages);
	resolveInstance->GetSlot("texture")->FlushUpdate();
}

void BloomPass::ExecuteSinglePass(ICommandList* computeCmdList, TIndex sourceMipLevel, TIndex destMipLevel) {
	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	Vector2ui sourceResolution = resolveRenderTarget.GetTargetImage(resourceIndex)->GetMipLevelSize2D(sourceMipLevel);
	Vector2ui destResolution = resolveRenderTarget.GetTargetImage(resourceIndex)->GetMipLevelSize2D(destMipLevel);

	const Vector2ui dispatchRes = {
		glm::max(sourceResolution.X, destResolution.X) / static_cast<TSize>(BLOCK_SIZE),
		glm::max(sourceResolution.Y, destResolution.Y) / static_cast<TSize>(BLOCK_SIZE)
	};

	computeCmdList->DispatchCompute({ dispatchRes.X, dispatchRes.Y, 1 });
}

void BloomPass::DownscaleBloom(GRAPHICS::ICommandList* computeCmdList) {
	computeCmdList->StartDebugSection("Downscale", Color::PURPLE());

	computeCmdList->BindMaterial(*downscaleMaterial);
	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	for (TIndex i = 0; i < GetNumPasses() - 1; i++) {
		computeCmdList->BindMaterialSlot(*downscalingMaterialInstance[i]->GetSlot("texture"));

		if (i == 0)
			computeCmdList->PushMaterialConstants("info", 1);
		else
			computeCmdList->PushMaterialConstants("info", 0);
		
		const TIndex sourceMipLevel = i;
		const TIndex destinationMipLevel = sourceMipLevel + 1;

		// Nivel anterior debe ser SAMPLED.
		computeCmdList->SetGpuImageBarrier(
			resolveRenderTarget.GetTargetImage(resourceIndex),
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ),
			{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = sourceMipLevel, .numMipLevels = 1 });

		// Nivel siguiente será escrito.
		// Al no haber sido usado previamente en este frame,
		// layout inicial es UNDEFINED y barrier es NONE.
		computeCmdList->SetGpuImageBarrier(
			resolveRenderTarget.GetTargetImage(resourceIndex),
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
	computeCmdList->StartDebugSection("Upscale", Color::PURPLE());

	computeCmdList->BindMaterial(*upscaleMaterial);
	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	for (TSize i = GetNumPasses() - 2; i > 0; i--) {
		computeCmdList->BindMaterialSlot(*upscalingMaterialInstance[i]->GetSlot("texture"));

		const TIndex sourceMipLevel = i + 1;
		const TIndex destinationMipLevel = i;

		// Nivel anterior debe ser SAMPLED.
		computeCmdList->SetGpuImageBarrier(
			resolveRenderTarget.GetTargetImage(resourceIndex),
			GpuImageLayout::SAMPLED,
			GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ),
			{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = sourceMipLevel, .numMipLevels = 1 });

		// Nivel siguiente será escrito (y leído).
		computeCmdList->SetGpuImageBarrier(
			resolveRenderTarget.GetTargetImage(resourceIndex),
			GpuImageLayout::GENERAL,
			GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ | GpuAccessStage::SHADER_WRITE),
			{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = destinationMipLevel, .numMipLevels = 1 });

		ExecuteSinglePass(computeCmdList, sourceMipLevel, destinationMipLevel);
	}

	computeCmdList->EndDebugSection();
}

void BloomPass::InitialCopy(ICommandList* computeCmdList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	computeCmdList->AddDebugMarker("Source Copy", Color::YELLOW());

	// La imagen de la escena se copia a la cadena de mipmaps:
	// debe estar en transfer source.
	computeCmdList->SetGpuImageBarrier(
		inputImages[resourceIndex],
		GpuImageLayout::TRANSFER_SOURCE,
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_READ));

	// La imagen de destino (mip 0) debe ser transfer dest. para efectuar la copia.
	// Al no haber sido usada previamente, el layout original es UNDEFINED y
	// barrier info es NONE.
	computeCmdList->SetGpuImageBarrier(
		resolveRenderTarget.GetTargetImage(resourceIndex),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE),
		{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = 1 });

	const Vector2ui imgSize = inputImages[resourceIndex]->GetSize2D();
	CopyImageInfo copyInfo = CopyImageInfo::CreateDefault2D(imgSize);
	copyInfo.destinationMipLevel = 0;

	computeCmdList->RawCopyImageToImage(
		*inputImages[resourceIndex],
		resolveRenderTarget.GetTargetImage(resourceIndex),
		copyInfo);
}

void BloomPass::Resolve(ICommandList* computeCmdList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	computeCmdList->StartDebugSection("Resolve", Color::PURPLE());

	// Source = nivel 1
	computeCmdList->SetGpuImageBarrier(
		resolveRenderTarget.GetTargetImage(resourceIndex),
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ),
		{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = 1, .numMipLevels = 1 });

	// Dest = nivel 0
	computeCmdList->SetGpuImageBarrier(
		resolveRenderTarget.GetTargetImage(resourceIndex),
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ | GpuAccessStage::SHADER_WRITE),
		{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = 1 });

	// La imagen de la escena se vuelve a usar.
	computeCmdList->SetGpuImageBarrier(
		inputImages[resourceIndex],
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ));

	const Vector2ui dispatchRes = {
		static_cast<TSize>(glm::ceil(resolveRenderTarget.GetSize().X / BLOCK_SIZE)),
		static_cast<TSize>(glm::ceil(resolveRenderTarget.GetSize().Y / BLOCK_SIZE))
	};

	computeCmdList->BindMaterial(*resolveMaterial);
	computeCmdList->BindMaterialSlot(*resolveInstance->GetSlot("texture"));
	computeCmdList->DispatchCompute({ dispatchRes.X, dispatchRes.Y, 1 });

	computeCmdList->EndDebugSection();
}

void BloomPass::Execute(ICommandList* computeCmdList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	computeCmdList->StartDebugSection("Bloom", Color::PURPLE());

	InitialCopy(computeCmdList);
	DownscaleBloom(computeCmdList);
	UpscaleBloom(computeCmdList);
	Resolve(computeCmdList);

	computeCmdList->EndDebugSection();
}

void BloomPass::UpdateMaterialInstance() {
	SetupMaterialInstances();
	// IPostProcessPass::UpdateMaterialInstance();
}

TSize BloomPass::GetNumPasses() const {
	return glm::min(
		resolveRenderTarget.GetTargetImage(0)->GetMipLevels(),
		maxNumPasses
	);
}
