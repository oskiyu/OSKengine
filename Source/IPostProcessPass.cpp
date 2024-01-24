#include "IPostProcessPass.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "Material.h"
#include "MaterialSystem.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void IPostProcessPass::Create(const Vector2ui& size) {
	RenderTargetAttachmentInfo info{};
	info.format = Format::RGBA16_SFLOAT;
	info.usage = GpuImageUsage::COMPUTE | GpuImageUsage::SAMPLED | GpuImageUsage::TRANSFER_SOURCE;
	info.sampler = GpuImageSamplerDesc::CreateDefault();
	resolveRenderTarget.Create(size, info);
	
	SetupDefaultMaterialInstances();
}

void IPostProcessPass::Resize(const Vector2ui& size) {
	resolveRenderTarget.Resize(size);
	
	SetupDefaultMaterialInstances();
}

void IPostProcessPass::SetInput(GpuImage* image, const GpuImageViewConfig& viewConfig) {
	inputImage = image;
	inputView = inputImage->GetView(viewConfig);

	if (postProcessingMaterialInstance.HasValue()) {
		postProcessingMaterialInstance->GetSlot("texture")->SetGpuImage("sceneImage", inputView);
	}
}

void IPostProcessPass::SetInputTarget(RenderTarget& target, const GpuImageViewConfig & viewConfig) {
	SetInput(target.GetMainColorImage(), viewConfig);
}

void IPostProcessPass::SetInputTarget(RtRenderTarget& target, const GpuImageViewConfig& viewConfig) {
	SetInput(target.GetTargetImage(), viewConfig);
}

ComputeRenderTarget& IPostProcessPass::GetOutput() {
	return resolveRenderTarget;
}

const ComputeRenderTarget& IPostProcessPass::GetOutput() const {
	return resolveRenderTarget;
}

void IPostProcessPass::SetupDefaultMaterialInstances() {
	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateStorage_Default();
	const auto* view = resolveRenderTarget.GetTargetImage()->GetView(viewConfig);

	if (postProcessingMaterialInstance.HasValue()) {
		postProcessingMaterialInstance->GetSlot("texture")->SetStorageImage("finalImage", view);
	}
}

void IPostProcessPass::UpdateMaterialInstance() {
	postProcessingMaterialInstance->GetSlot("texture")->FlushUpdate();
}
