#include "ToneMapping.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "MaterialSystem.h"
#include "Material.h"
#include "GpuImageLayout.h"
#include "IGpuStorageBuffer.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void ToneMappingPass::Create(const Vector2ui& size) {
	postProcessingMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PostProcess/tonemapping.json");
	postProcessingMaterialInstance = postProcessingMaterial->CreateInstance().GetPointer();

	IPostProcessPass::Create(size);
}

void ToneMappingPass::Execute(ICommandList* computeCmdList) {
	const TSize resourceIndex = Engine::GetRenderer()->GetCurrentResourceIndex();

	computeCmdList->StartDebugSection("Tone Mapping", Color::PURPLE());

	computeCmdList->SetGpuImageBarrier(
		resolveRenderTarget.GetTargetImage(resourceIndex), 
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ), 
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE));

	computeCmdList->BindMaterial(postProcessingMaterial);
	computeCmdList->BindMaterialSlot(postProcessingMaterialInstance->GetSlot("texture"));
	computeCmdList->BindMaterialSlot(postProcessingMaterialInstance->GetSlot("exposure"));

	const Vector3ui dispatchRes = {
		static_cast<TSize>(glm::ceil(resolveRenderTarget.GetSize().X / 8.0f)),
		static_cast<TSize>(glm::ceil(resolveRenderTarget.GetSize().Y / 8.0f)),
		1
	};

	struct { float exposure; float gamma; } const pushConstants{ exposure, gamma - 1.2f };
	computeCmdList->PushMaterialConstants("gamma", pushConstants);
	computeCmdList->DispatchCompute(dispatchRes);

	computeCmdList->SetGpuImageBarrier(
		resolveRenderTarget.GetTargetImage(resourceIndex), 
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuBarrierStage::COMPUTE_SHADER, GpuBarrierAccessStage::SHADER_WRITE), 
		GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ));

	computeCmdList->EndDebugSection();
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
	postProcessingMaterialInstance->GetSlot("exposure")->SetStorageBuffers("exposure", (const GpuDataBuffer**)buffers);
	postProcessingMaterialInstance->GetSlot("exposure")->FlushUpdate();
}
