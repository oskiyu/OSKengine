#include "SmaaPass.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "MaterialSystem.h"
#include "Material.h"
#include "GpuImageLayout.h"
#include "IGpuImage.h"
#include "GpuImageDimensions.h"
#include "GpuMemoryTypes.h"
#include "GpuImageSamplerDesc.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void SmaaPass::Create(const Vector2ui& size) {
	postProcessingMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/PostProcess/SMAA/edge_detection.json");
	postProcessingMaterialInstance = postProcessingMaterial->CreateInstance().GetPointer();

	IPostProcessPass::Create(size);

	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		GpuImageSamplerDesc sampler{};
		sampler.mipMapMode = GpuImageMipmapMode::NONE;
		sampler.addressMode = GpuImageAddressMode::EDGE;

		edgeImages[i] = Engine::GetRenderer()->GetAllocator()->CreateImage({ size.X, size.Y, 1 }, GpuImageDimension::d2D, 1, Format::RGBA32_SFLOAT,
			GpuImageUsage::SAMPLED | GpuImageUsage::SAMPLED_ARRAY | GpuImageUsage::COMPUTE | GpuImageUsage::TRANSFER_SOURCE, GpuSharedMemoryType::GPU_ONLY, 1, sampler).GetPointer();
	}

	resolveRenderTarget.SetName("SMAA Output");
}

void SmaaPass::Resize(const Vector2ui& size) {
	IPostProcessPass::Resize(size);

	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		GpuImageSamplerDesc sampler{};
		sampler.mipMapMode = GpuImageMipmapMode::NONE;
		sampler.addressMode = GpuImageAddressMode::EDGE;

		edgeImages[i] = Engine::GetRenderer()->GetAllocator()->CreateImage({ size.X, size.Y, 1 }, GpuImageDimension::d2D, 1, Format::RGBA32_SFLOAT,
			GpuImageUsage::SAMPLED | GpuImageUsage::SAMPLED_ARRAY | GpuImageUsage::COMPUTE | GpuImageUsage::TRANSFER_SOURCE, GpuSharedMemoryType::GPU_ONLY, 1, sampler).GetPointer();
	}
}

void SmaaPass::SetupMaterials() {

}

void SmaaPass::Execute(ICommandList* computeCmdList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	computeCmdList->SetGpuImageBarrier(inputImages[resourceIndex], GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ),
		{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
	computeCmdList->SetGpuImageBarrier(resolveRenderTarget.GetMainTargetImage(resourceIndex), GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE),
		{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	computeCmdList->BindMaterial(postProcessingMaterial);
	computeCmdList->BindMaterialSlot(postProcessingMaterialInstance->GetSlot("texture"));

	const Vector3ui dispatchRes = {
		static_cast<TSize>(glm::ceil(resolveRenderTarget.GetSize().X / 8.0f)),
		static_cast<TSize>(glm::ceil(resolveRenderTarget.GetSize().Y / 8.0f)),
		1
	};

	computeCmdList->DispatchCompute(dispatchRes);

	computeCmdList->SetGpuImageBarrier(inputImages[resourceIndex], GpuImageLayout::SHADER_READ_ONLY,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
	computeCmdList->SetGpuImageBarrier(resolveRenderTarget.GetMainTargetImage(resourceIndex), GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ),
		{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
}
