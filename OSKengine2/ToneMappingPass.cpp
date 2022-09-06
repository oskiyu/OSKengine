#include "ToneMapping.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "MaterialSystem.h"
#include "Material.h"
#include "GpuImageLayout.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void ToneMappingPass::Create(const Vector2ui& size) {
	postProcessingMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/PostProcess/tonemapping.json");
	postProcessingMaterialInstance = postProcessingMaterial->CreateInstance().GetPointer();

	IPostProcessPass::Create(size);

	resolveRenderTarget.SetName("Tone Mapping Output");
}

void ToneMappingPass::Execute(ICommandList* computeCmdList) {
	const TSize frameIndex = Engine::GetRenderer()->GetCurrentCommandListIndex();

	computeCmdList->SetGpuImageBarrier(inputImages[frameIndex], GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ),
		{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
	computeCmdList->SetGpuImageBarrier(resolveRenderTarget.GetMainTargetImage(frameIndex), GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE),
		{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });

	computeCmdList->BindMaterial(postProcessingMaterial);
	computeCmdList->BindMaterialSlot(postProcessingMaterialInstance->GetSlot("texture"));
	computeCmdList->BindMaterialSlot(postProcessingMaterialInstance->GetSlot("exposure"));

	const Vector3ui dispatchRes = {
		static_cast<TSize>(glm::ceil(resolveRenderTarget.GetSize().X / 32.f)),
		static_cast<TSize>(glm::ceil(resolveRenderTarget.GetSize().Y / 32.f)),
		1
	};

	struct { float exposure; float gamma; } const pushConstants{ exposure, gamma - 1.2f };
	computeCmdList->PushMaterialConstants("gamma", pushConstants);
	computeCmdList->DispatchCompute(dispatchRes);

	computeCmdList->SetGpuImageBarrier(inputImages[frameIndex], GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_READ), GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE),
		{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
	computeCmdList->SetGpuImageBarrier(resolveRenderTarget.GetMainTargetImage(frameIndex), GpuImageLayout::SHADER_READ_ONLY,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
		{ .baseLayer = 0, .numLayers = ALL_IMAGE_LAYERS, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
}

void ToneMappingPass::SetExposure(float exposure) {
	this->exposure = exposure;
}

float ToneMappingPass::GetExposure() const {
	return exposure;
}

void ToneMappingPass::SetGamma(float gamma) {
	this->gamma = gamma;
}

float ToneMappingPass::GetGamma() const {
	return gamma;
}

void ToneMappingPass::SetExposureBuffers(const IGpuStorageBuffer* buffers[3]) {
	postProcessingMaterialInstance->GetSlot("exposure")->SetStorageBuffers("exposure", buffers);
	postProcessingMaterialInstance->GetSlot("exposure")->FlushUpdate();
}
