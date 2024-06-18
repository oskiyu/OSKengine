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
	OSK_ASSERT(false, NotImplementedException());

	// postProcessingMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PostProcess/SMAA/edge_detection.json");
	// postProcessingMaterialInstance = postProcessingMaterial->CreateInstance().GetPointer();

	IPostProcessPass::Create(size);

	for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		GpuImageSamplerDesc sampler{};
		sampler.mipMapMode = GpuImageMipmapMode::NONE;
		sampler.addressMode = GpuImageAddressMode::EDGE;

		//edgeImages[i] = Engine::GetRenderer()->GetAllocator()->CreateImage({ size.X, size.Y, 1 }, GpuImageDimension::d2D, 1, Format::RGBA32_SFLOAT,
			//GpuImageUsage::SAMPLED | GpuImageUsage::SAMPLED_ARRAY | GpuImageUsage::COMPUTE | GpuImageUsage::TRANSFER_SOURCE, GpuSharedMemoryType::GPU_ONLY, 1, sampler).GetPointer();
	}

	// resolveRenderTarget.SetName("SMAA Output");
}

void SmaaPass::Resize(const Vector2ui& size) {
	IPostProcessPass::Resize(size);

	for (UIndex32 i = 0; i < MAX_RESOURCES_IN_FLIGHT; i++) {
		GpuImageSamplerDesc sampler{};
		sampler.mipMapMode = GpuImageMipmapMode::NONE;
		sampler.addressMode = GpuImageAddressMode::EDGE;

		//edgeImages[i] = Engine::GetRenderer()->GetAllocator()->CreateImage({ size.X, size.Y, 1 }, GpuImageDimension::d2D, 1, Format::RGBA32_SFLOAT,
			//GpuImageUsage::SAMPLED | GpuImageUsage::SAMPLED_ARRAY | GpuImageUsage::COMPUTE | GpuImageUsage::TRANSFER_SOURCE, GpuSharedMemoryType::GPU_ONLY, 1, sampler).GetPointer();
	}
}

void SmaaPass::SetupMaterials() {

}

void SmaaPass::Execute(ICommandList* computeCmdList) {
	OSK_ASSERT(false, NotImplementedException());
	/*computeCmdList->SetGpuImageBarrier(inputImage, GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SHADER_READ), 
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ),
		{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	computeCmdList->SetGpuImageBarrier(resolveRenderTarget.GetTargetImage(), GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ), 
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE),
		{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	computeCmdList->BindMaterial(*postProcessingMaterial);
	computeCmdList->BindMaterialSlot(*postProcessingMaterialInstance->GetSlot("texture"));

	const Vector3ui dispatchRes = {
		static_cast<USize32>(glm::ceil(resolveRenderTarget.GetSize().x / 8.0f)),
		static_cast<USize32>(glm::ceil(resolveRenderTarget.GetSize().y / 8.0f)),
		1
	};

	computeCmdList->DispatchCompute(dispatchRes);

	computeCmdList->SetGpuImageBarrier(
		inputImage, 
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ), 
		GpuBarrierInfo(GpuCommandStage::FRAGMENT_SHADER, GpuAccessStage::SHADER_READ),
		{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	computeCmdList->SetGpuImageBarrier(
		resolveRenderTarget.GetTargetImage(), 
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE), 
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_READ),
		{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
		*/
}
