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

#include <ext/matrix_clip_space.hpp>
#include <ext/matrix_transform.hpp>

using namespace OSK;
using namespace OSK::GRAPHICS;

constexpr float BLOCK_SIZE = 8.f;

void BloomPass::Create(const Vector2ui& size) {
	downscaleMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/Bloom/downscale.json");
	upscaleMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/Bloom/upscale.json");
	resolveMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/Bloom/final.json");

	intermediateInstances[0] = downscaleMaterial->CreateInstance().GetPointer();
	intermediateInstances[1] = downscaleMaterial->CreateInstance().GetPointer();
	resolveInstance = resolveMaterial->CreateInstance().GetPointer();

	RenderTargetAttachmentInfo intermediateInfo{};
	intermediateInfo.format = Format::RGBA16_SFLOAT;
	intermediateInfo.sampler = GpuImageSamplerDesc::CreateDefault();
	intermediateInfo.sampler.filteringType = GpuImageFilteringType::LIENAR;
	intermediateInfo.sampler.addressMode = GpuImageAddressMode::EDGE;
	intermediateInfo.usage = GpuImageUsage::COMPUTE | GpuImageUsage::SAMPLED | GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION;
	intermediateInfo.name = "Bloom Target 0";
	bloomIntermediateTargets[0].Create(size, intermediateInfo);

	intermediateInfo.name = "Bloom Target 1";
	bloomIntermediateTargets[1].Create(size, intermediateInfo);

	RenderTargetAttachmentInfo resolveInfo{};
	resolveInfo.format = Format::RGBA16_SFLOAT;
	resolveInfo.name = "Bloom Output";
	resolveInfo.usage = GpuImageUsage::TRANSFER_SOURCE | GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::COMPUTE | GpuImageUsage::SAMPLED;
	resolveRenderTarget.Create(size, resolveInfo);
}

void BloomPass::Resize(const Vector2ui& size) {
	bloomIntermediateTargets[0].Resize(size);
	bloomIntermediateTargets[1].Resize(size);

	resolveRenderTarget.Resize(size);
}

void BloomPass::SetupMaterialInstances() {
	for (TIndex target = 0; target < 2; target++) {
		const TIndex source = target;
		const TIndex destination = (target + 1) % 2;

		const GpuImage* sourceImages[NUM_RESOURCES_IN_FLIGHT]{};
		for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
			sourceImages[i] = bloomIntermediateTargets[source].GetTargetImage(i);

		const GpuImage* targetImages[NUM_RESOURCES_IN_FLIGHT]{};
		for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
			targetImages[i] = bloomIntermediateTargets[destination].GetTargetImage(i);

		intermediateInstances[target]->GetSlot("texture")->SetGpuImages("inputImg", sourceImages);
		intermediateInstances[target]->GetSlot("texture")->SetStorageImages("outputImg", targetImages);
		intermediateInstances[target]->GetSlot("texture")->FlushUpdate();
	}

	// Resolve
	const GpuImage* sourceImages[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		sourceImages[i] = bloomIntermediateTargets[lastDestination].GetTargetImage(i);

	const GpuImage* sceneImages[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		sceneImages[i] = inputImages[i];

	const GpuImage* targetImages[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		targetImages[i] = resolveRenderTarget.GetTargetImage(i);

	resolveInstance->GetSlot("texture")->SetGpuImages("bloomImg", sourceImages);
	resolveInstance->GetSlot("texture")->SetGpuImages("sceneImg", sceneImages);
	resolveInstance->GetSlot("texture")->SetStorageImages("outputImg", targetImages);
	resolveInstance->GetSlot("texture")->FlushUpdate();
}

void BloomPass::ExecuteSinglePass(ICommandList* computeCmdList, const Vector2f& oldRes, const Vector2f& newRes, TSize source, TSize destination) {
	struct {
		Vector4f resolutions;
	} const pushConst {
		.resolutions = {
			oldRes.X, oldRes.Y,
			newRes.X, newRes.Y
		}
	};

	computeCmdList->BindMaterialSlot(intermediateInstances[source]->GetSlot("texture"));
	computeCmdList->PushMaterialConstants("info", pushConst);

	const Vector2ui dispatchRes = {
		static_cast<TSize>(glm::ceil((glm::max(oldRes.X, newRes.X)) / BLOCK_SIZE)),
		static_cast<TSize>(glm::ceil((glm::max(oldRes.Y, newRes.Y)) / BLOCK_SIZE))
	};

	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	computeCmdList->SetGpuImageBarrier(bloomIntermediateTargets[source].GetTargetImage(resourceIndex), GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ));
	computeCmdList->SetGpuImageBarrier(bloomIntermediateTargets[destination].GetTargetImage(resourceIndex), GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE));

	computeCmdList->DispatchCompute({ dispatchRes.X, dispatchRes.Y, 1 });
}

void BloomPass::DownscaleBloom(GRAPHICS::ICommandList* computeCmdList, Vector2f* res) {
	computeCmdList->BindMaterial(downscaleMaterial);

	for (TIndex i = 0; i < numPasses; i++) {
		const TIndex source = (firstSource + i) % 2;
		const TIndex destination = (firstDestination + i) % 2;

		ExecuteSinglePass(computeCmdList, *res, *res / 2.0f, source, destination);

		*res /= 2.0f;
	}
}

void BloomPass::UpscaleBloom(GRAPHICS::ICommandList* computeCmdList, Vector2f* res) {
	computeCmdList->BindMaterial(upscaleMaterial);
	for (TSize i = 0; i < numPasses; i++) {
		const TIndex source = (firstSource + i) % 2;
		const TIndex destination = (firstDestination + i) % 2;
		
		ExecuteSinglePass(computeCmdList, *res, *res * 2.0f, source, destination);

		*res *= 2.0f;
	}
}

void BloomPass::Execute(ICommandList* computeCmdList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	// Copiamos la imagen de la escena a la primera imagen de source

	computeCmdList->StartDebugSection("Bloom", Color::PURPLE());

	computeCmdList->AddDebugMarker("Source Copy", Color::YELLOW());

	computeCmdList->SetGpuImageBarrier(inputImages[resourceIndex], GpuImageLayout::TRANSFER_SOURCE,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_READ));
	computeCmdList->SetGpuImageBarrier(bloomIntermediateTargets[firstSource].GetTargetImage(resourceIndex), GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE));

	const Vector2ui imgSize = { inputImages[resourceIndex]->GetSize().X, inputImages[resourceIndex]->GetSize().Y };
	CopyImageInfo copyInfo = CopyImageInfo::CreateDefault2D(imgSize);
	computeCmdList->CopyImageToImage(inputImages[resourceIndex], bloomIntermediateTargets[firstSource].GetTargetImage(resourceIndex), copyInfo);


	computeCmdList->SetGpuImageBarrier(inputImages[resourceIndex], GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_READ), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ));

	// Source = read
	computeCmdList->SetGpuImageBarrier(bloomIntermediateTargets[firstSource].GetTargetImage(resourceIndex), GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ));
	// Target = write
	computeCmdList->SetGpuImageBarrier(bloomIntermediateTargets[firstDestination].GetTargetImage(resourceIndex), GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE));


	computeCmdList->StartDebugSection("Downscale", Color::PURPLE());

	// Procesado
	Vector2f initialBloomRes = resolveRenderTarget.GetSize().ToVector2f();
	DownscaleBloom(computeCmdList, &initialBloomRes);
	computeCmdList->EndDebugSection();

	computeCmdList->StartDebugSection("Upscale", Color::PURPLE());
	UpscaleBloom(computeCmdList, &initialBloomRes);
	computeCmdList->EndDebugSection();

	// Resolve
	computeCmdList->StartDebugSection("Resolve", Color::PURPLE());

	computeCmdList->SetGpuImageBarrier(bloomIntermediateTargets[lastSource].GetTargetImage(resourceIndex), GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ));
	computeCmdList->SetGpuImageBarrier(bloomIntermediateTargets[lastDestination].GetTargetImage(resourceIndex), GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE));

	computeCmdList->SetGpuImageBarrier(resolveRenderTarget.GetTargetImage(resourceIndex), GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE));

	const Vector2ui dispatchRes = {
		static_cast<TSize>(glm::ceil(resolveRenderTarget.GetSize().X / BLOCK_SIZE)),
		static_cast<TSize>(glm::ceil(resolveRenderTarget.GetSize().Y / BLOCK_SIZE))
	};
	computeCmdList->BindMaterial(resolveMaterial);
	computeCmdList->BindMaterialSlot(resolveInstance->GetSlot("texture"));
	computeCmdList->DispatchCompute({ dispatchRes.X, dispatchRes.Y, 1 });

	computeCmdList->SetGpuImageBarrier(resolveRenderTarget.GetTargetImage(resourceIndex), GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ));

	computeCmdList->EndDebugSection();
	computeCmdList->EndDebugSection();
}

void BloomPass::UpdateMaterialInstance() {
	SetupMaterialInstances();
	// IPostProcessPass::UpdateMaterialInstance();
}
