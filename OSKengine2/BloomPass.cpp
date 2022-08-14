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

BloomPass::BloomPass() {
	bloomMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/material_bloom.json");
	for (TSize i = 0; i < _countof(bloomMaterialInstances); i++)
		bloomMaterialInstances[i] = bloomMaterial->CreateInstance().GetPointer();
}

void BloomPass::SetInput(const GpuImage* images[3]) {
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		bloomMaterialInstances[i]->GetSlot("texture")->SetGpuImages("sceneImage", images);
		bloomMaterialInstances[i]->GetSlot("texture")->FlushUpdate();
	}
}

void BloomPass::SetInput(const RenderTarget& target) {
	const GpuImage* images[3]{};
	for (TSize i = 0; i < 3; i++)
		images[i] = target.GetMainTargetImage(i);

	SetInput(images);
}

void BloomPass::Create(const Vector2ui& size) {
	for (TSize i = 0; i < _countof(bloomTargets); i++) {
		GpuImageSamplerDesc sampler{};
		sampler.mipMapMode = GpuImageMipmapMode::NONE;

		bloomTargets[i] = Engine::GetRenderer()->GetMemoryAllocator()
			->CreateImage({ size.X, size.Y, 1 }, GpuImageDimension::d2D, 2, Format::RGBA32_SFLOAT,
				GpuImageUsage::SAMPLED | GpuImageUsage::SAMPLED_ARRAY | GpuImageUsage::COMPUTE | GpuImageUsage::TRANSFER_SOURCE, GpuSharedMemoryType::GPU_ONLY, 1, sampler).GetPointer();

		bloomTargets[i]->SetDebugName("Bloom Target " + std::to_string(i));
	}

	resolveRenderTarget.SetTargetImageUsage(GpuImageUsage::TRANSFER_DESTINATION);
	resolveRenderTarget.Create(size, Format::RGBA32_SFLOAT, Format::D32S8_SFLOAT_SUINT);

	SetupMaterialInstances();
}

void BloomPass::Resize(const Vector2ui& size) {
	for (TSize i = 0; i < _countof(bloomTargets); i++) {
		GpuImageSamplerDesc sampler{};
		sampler.mipMapMode = GpuImageMipmapMode::NONE;

		bloomTargets[i] = Engine::GetRenderer()->GetMemoryAllocator()
			->CreateImage({ size.X, size.Y, 1 }, GpuImageDimension::d2D, 2, Format::RGBA32_SFLOAT,
				GpuImageUsage::SAMPLED | GpuImageUsage::SAMPLED_ARRAY | GpuImageUsage::COMPUTE | GpuImageUsage::TRANSFER_SOURCE, GpuSharedMemoryType::GPU_ONLY, 1, sampler).GetPointer();

		bloomTargets[i]->SetDebugName("Bloom Target " + std::to_string(i));
	}

	resolveRenderTarget.Resize(size);

	SetupMaterialInstances();
}

void BloomPass::SetupMaterialInstances() {
	const GpuImage* images[NUM_RESOURCES_IN_FLIGHT]{};

	for (TSize frameInstance = 0; frameInstance < NUM_RESOURCES_IN_FLIGHT; frameInstance++) {
		for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
			images[i] = bloomTargets[i].GetPointer();

		bloomMaterialInstances[frameInstance]->GetSlot("texture")->SetStorageImages("bloomImage", images, SampledArrayType::ARRAY);
		bloomMaterialInstances[frameInstance]->GetSlot("texture")->FlushUpdate();
	}
}

void BloomPass::ExecuteSinglePass(ICommandList* computeCmdList, const Vector2f& oldRes, const Vector2f& newRes, TSize inputIndex, TSize outputIndex) {
	struct {
		Vector4f resolutions;
		Vector2i indices;
	} const pushConst{
		.resolutions = {
			oldRes.X, oldRes.Y,
			newRes.X, newRes.Y
		},
		.indices = {
			static_cast<int>(inputIndex),
			static_cast<int>(outputIndex)
		}
	};

	computeCmdList->BindMaterial(bloomMaterial);
	computeCmdList->BindMaterialSlot(bloomMaterialInstances[inputIndex]->GetSlot("texture"));
	computeCmdList->PushMaterialConstants("info", pushConst);

	Vector2ui dispatchRes = {
		static_cast<TSize>(glm::max(oldRes.X, newRes.X)) / 16,
		static_cast<TSize>(glm::max(oldRes.Y, newRes.Y)) / 16
	};

	computeCmdList->SetGpuImageBarrier(bloomTargets[Engine::GetRenderer()->GetCurrentCommandListIndex()].GetPointer(), GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE),
		{ .baseLayer = outputIndex, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
	computeCmdList->DispatchCompute({ dispatchRes.X, dispatchRes.Y, 1 });
	computeCmdList->SetGpuImageBarrier(bloomTargets[Engine::GetRenderer()->GetCurrentCommandListIndex()].GetPointer(), GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ),
		{ .baseLayer = outputIndex, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
}

void BloomPass::DownscaleBloom(GRAPHICS::ICommandList* computeCmdList, Vector2f* res) {
	for (TSize i = 0; i < 4; i++) {
		const TSize inputIndex = i == 0 ? 2 : i % 2;
		const TSize outputIndex = i % 2 == 0 ? 1 : 0;

		ExecuteSinglePass(computeCmdList, *res, *res / 2.0f, inputIndex, outputIndex);

		*res /= 2.0f;
	}
}

void BloomPass::UpscaleBloom(GRAPHICS::ICommandList* computeCmdList, Vector2f* res) {
	for (TSize i = 0; i < 4; i++) {
		const TSize inputIndex = i % 2;
		const TSize outputIndex = inputIndex == 0 ? 1 : 0;

		ExecuteSinglePass(computeCmdList, *res, *res * 2.0f, inputIndex, outputIndex);

		*res *= 2.0f;
	}
}

void BloomPass::Execute(ICommandList* computeCmdList) {
	const TSize frameIndex = Engine::GetRenderer()->GetCurrentCommandListIndex();

	computeCmdList->SetGpuImageBarrier(bloomTargets[frameIndex].GetPointer(), GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_READ), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ),
		{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	Vector2f initialBloomRes = resolveRenderTarget.GetSize().ToVector2f();
	DownscaleBloom(computeCmdList, &initialBloomRes);
	UpscaleBloom(computeCmdList, &initialBloomRes);

	computeCmdList->SetGpuImageBarrier(bloomTargets[frameIndex].GetPointer(), GpuImageLayout::SHADER_READ_ONLY,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	// Resolve / copy
	computeCmdList->SetGpuImageBarrier(bloomTargets[frameIndex].GetPointer(), GpuImageLayout::TRANSFER_SOURCE,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_READ));
	computeCmdList->SetGpuImageBarrier(resolveRenderTarget.GetMainTargetImage(frameIndex), GpuImageLayout::TRANSFER_DESTINATION,
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE));

	computeCmdList->CopyImageToImage(bloomTargets[frameIndex].GetPointer(), resolveRenderTarget.GetMainTargetImage(frameIndex),
		1, 0, 0, 0, 0, resolveRenderTarget.GetSize());

	computeCmdList->SetGpuImageBarrier(resolveRenderTarget.GetMainTargetImage(frameIndex), GpuImageLayout::SHADER_READ_ONLY,
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ));
}

const RenderTarget& BloomPass::GetRenderTarget() const {
	return resolveRenderTarget;
}
