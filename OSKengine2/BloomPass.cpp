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

constexpr float BLOCK_SIZE = 4.f;

void BloomPass::Create(const Vector2ui& size) {
	downscaleMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/Bloom/downscale.json");
	upscaleMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/Bloom/upscale.json");
	finalMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/Bloom/final.json");

	postProcessingMaterialInstance = downscaleMaterial->CreateInstance().GetPointer();

	for (TSize i = 0; i < _countof(bloomTargets); i++) {
		GpuImageSamplerDesc sampler{};
		sampler.mipMapMode = GpuImageMipmapMode::NONE;

		bloomTargets[i] = Engine::GetRenderer()->GetAllocator()
			->CreateImage({ size.X, size.Y, 1 }, GpuImageDimension::d2D, numPasses, Format::RGBA32_SFLOAT,
				GpuImageUsage::SAMPLED | GpuImageUsage::SAMPLED_ARRAY | GpuImageUsage::COMPUTE | GpuImageUsage::TRANSFER_SOURCE, GpuSharedMemoryType::GPU_ONLY, 1, sampler).GetPointer();

		bloomTargets[i]->SetDebugName("Bloom Target " + std::to_string(i));
	}

	RenderTargetAttachmentInfo resolveInfo{};
	resolveInfo.format = Format::RGBA32_SFLOAT;
	resolveInfo.name = "Bloom Output";
	resolveInfo.usage = GpuImageUsage::TRANSFER_DESTINATION | GpuImageUsage::COMPUTE |GpuImageUsage::SAMPLED;
	resolveRenderTarget.Create(size, resolveInfo);
}

void BloomPass::Resize(const Vector2ui& size) {
	for (TSize i = 0; i < _countof(bloomTargets); i++) {
		GpuImageSamplerDesc sampler{};
		sampler.mipMapMode = GpuImageMipmapMode::NONE;
		sampler.addressMode = GpuImageAddressMode::EDGE;

		bloomTargets[i] = Engine::GetRenderer()->GetAllocator()
			->CreateImage({ size.X, size.Y, 1 }, GpuImageDimension::d2D, numPasses, Format::RGBA32_SFLOAT,
				GpuImageUsage::SAMPLED | GpuImageUsage::SAMPLED_ARRAY | GpuImageUsage::COMPUTE | GpuImageUsage::TRANSFER_SOURCE, GpuSharedMemoryType::GPU_ONLY, 1, sampler).GetPointer();

		bloomTargets[i]->SetDebugName("Bloom Target " + std::to_string(i));
	}

	resolveRenderTarget.Resize(size);
}

void BloomPass::SetupMaterialInstances() {
	const GpuImage* images[NUM_RESOURCES_IN_FLIGHT]{};

	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = bloomTargets[i].GetPointer();

	postProcessingMaterialInstance->GetSlot("texture")->SetStorageImages("bloomImage", images, SampledArrayType::ARRAY);
	postProcessingMaterialInstance->GetSlot("texture")->FlushUpdate();
}

void BloomPass::ExecuteSinglePass(ICommandList* computeCmdList, const Vector2f& oldRes, const Vector2f& newRes, TSize inputIndex, TSize outputIndex) {
	struct {
		Vector4f resolutions;
		Vector2i indices;
	} const pushConst {
		.resolutions = {
			oldRes.X, oldRes.Y,
			newRes.X, newRes.Y
		},
		.indices = {
			static_cast<int>(inputIndex),
			static_cast<int>(outputIndex)
		}
	};

	computeCmdList->BindMaterialSlot(postProcessingMaterialInstance->GetSlot("texture"));
	computeCmdList->BindMaterialSlot(postProcessingMaterialInstance->GetSlot("exposure"));
	computeCmdList->PushMaterialConstants("info", pushConst);

	Vector2ui dispatchRes = {
		static_cast<TSize>(glm::ceil((glm::max(oldRes.X, newRes.X)) / BLOCK_SIZE)),
		static_cast<TSize>(glm::ceil((glm::max(oldRes.Y, newRes.Y)) / BLOCK_SIZE))
	};

	const TIndex resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	computeCmdList->SetGpuImageBarrier(bloomTargets[resourceIndex].GetPointer(), GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE),
		{ .baseLayer = outputIndex, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
	computeCmdList->DispatchCompute({ dispatchRes.X, dispatchRes.Y, 1 });
	computeCmdList->SetGpuImageBarrier(bloomTargets[resourceIndex].GetPointer(), GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ),
		{ .baseLayer = outputIndex, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
}

void BloomPass::DownscaleBloom(GRAPHICS::ICommandList* computeCmdList, Vector2f* res) {
	computeCmdList->BindMaterial(downscaleMaterial);

	for (TSize i = 0; i < numPasses; i++) {
		TSize inputIndex = i - 1;
		TSize outputIndex = i;

		if (i == 0) {
			inputIndex = numPasses;
			outputIndex = 0;
		}

		ExecuteSinglePass(computeCmdList, *res, *res / 2.0f, inputIndex, outputIndex);

		*res /= 2.0f;
	}
}

void BloomPass::UpscaleBloom(GRAPHICS::ICommandList* computeCmdList, Vector2f* res) {
	computeCmdList->BindMaterial(upscaleMaterial);
	for (TSize i = numPasses - 1; i > 0; i--) {
		const TSize inputIndex = i;
		const TSize outputIndex = inputIndex - 1;
		
		ExecuteSinglePass(computeCmdList, *res, *res * 2.0f, inputIndex, outputIndex);

		*res *= 2.0f;
	}

	computeCmdList->BindMaterial(finalMaterial);
	ExecuteSinglePass(computeCmdList, *res, *res * 2.0f, 0, 3);
}

void BloomPass::Execute(ICommandList* computeCmdList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	computeCmdList->SetGpuImageBarrier(bloomTargets[resourceIndex].GetPointer(), GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_READ), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ),
		{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	Vector2f initialBloomRes = resolveRenderTarget.GetSize().ToVector2f();
	DownscaleBloom(computeCmdList, &initialBloomRes);
	UpscaleBloom(computeCmdList, &initialBloomRes);

	computeCmdList->SetGpuImageBarrier(bloomTargets[resourceIndex].GetPointer(), GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	// Resolve / copy
	computeCmdList->SetGpuImageBarrier(bloomTargets[resourceIndex].GetPointer(), GpuImageLayout::TRANSFER_SOURCE,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_READ));
	computeCmdList->SetGpuImageBarrier(resolveRenderTarget.GetTargetImage(resourceIndex), GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE));

	computeCmdList->CopyImageToImage(bloomTargets[resourceIndex].GetPointer(), resolveRenderTarget.GetTargetImage(resourceIndex),
		1, 3, 0, 0, 0, resolveRenderTarget.GetSize());

	computeCmdList->SetGpuImageBarrier(resolveRenderTarget.GetTargetImage(resourceIndex), GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ));
}

void BloomPass::SetExposureBuffers(const IGpuStorageBuffer* buffers[3]) {
	postProcessingMaterialInstance->GetSlot("exposure")->SetStorageBuffers("exposure", (const GpuDataBuffer**)buffers);
	postProcessingMaterialInstance->GetSlot("exposure")->FlushUpdate();
}

void BloomPass::UpdateMaterialInstance() {
	SetupMaterialInstances();
	IPostProcessPass::UpdateMaterialInstance();
}
