#include "ToneMapping.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "MaterialSystem.h"
#include "Material.h"
#include "GpuImageLayout.h"
#include "GpuBuffer.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void ToneMappingPass::Create(const Vector2ui& size) {
	postProcessingMaterial = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PostProcess/tonemapping.json");
	postProcessingMaterialInstance = postProcessingMaterial->CreateInstance().GetPointer();

	IPostProcessPass::Create(size);
}

void ToneMappingPass::Execute(ICommandList* computeCmdList) {
	computeCmdList->StartDebugSection("Tone Mapping", Color::Purple);

	computeCmdList->SetGpuImageBarrier(
		inputImage,
		GpuImageLayout::SAMPLED,
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SAMPLED_READ),
		{ .baseLayer = 0, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = 1 });

	computeCmdList->SetGpuImageBarrier(
		resolveRenderTarget.GetTargetImage(),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::GENERAL,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::COMPUTE_SHADER, GpuAccessStage::SHADER_WRITE));

	computeCmdList->BindMaterial(*postProcessingMaterial);
	computeCmdList->BindMaterialSlot(*postProcessingMaterialInstance->GetSlot("texture"));
	computeCmdList->BindMaterialSlot(*postProcessingMaterialInstance->GetSlot("exposure"));

	const Vector3ui dispatchRes = {
		static_cast<USize32>(glm::ceil(resolveRenderTarget.GetSize().x / 8.0f)),
		static_cast<USize32>(glm::ceil(resolveRenderTarget.GetSize().y / 8.0f)),
		1
	};

	struct { float exposure; float gamma; } const pushConstants{ exposure, gamma - 1.2f };
	computeCmdList->PushMaterialConstants("gamma", pushConstants);
	computeCmdList->DispatchCompute(dispatchRes);

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

void ToneMappingPass::SetExposureBuffers(std::span<const GpuBuffer*, MAX_RESOURCES_IN_FLIGHT> buffers) {
	postProcessingMaterialInstance->GetSlot("exposure")->SetStorageBuffers("exposure", buffers);
	postProcessingMaterialInstance->GetSlot("exposure")->FlushUpdate();
}
