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

void IPostProcessPass::SetInput(std::span<GpuImage*, NUM_RESOURCES_IN_FLIGHT> images, const GpuImageViewConfig& viewConfig) {
	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> imgs{};

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		imgs[i] = images[i]->GetView(viewConfig);
		inputImages[i] = images[i];
		inputViews[i] = imgs[i];
	}

	if (postProcessingMaterialInstance.HasValue())
		postProcessingMaterialInstance->GetSlot("texture")->SetGpuImages("sceneImage", imgs);
}

void IPostProcessPass::SetInput(const RenderTarget& target, const GpuImageViewConfig & viewConfig, InputType type) {
	std::array<GpuImage*, NUM_RESOURCES_IN_FLIGHT> images{};

	for (UIndex32 i = 0; i < images.size(); i++)
		images[i] = target.GetMainColorImage(i);

	SetInput(images, viewConfig);
}

void IPostProcessPass::SetInput(const RtRenderTarget& target, const GpuImageViewConfig& viewConfig, InputType type) {
	std::array<GpuImage*, NUM_RESOURCES_IN_FLIGHT> images{};

	for (UIndex32 i = 0; i < images.size(); i++)
		images[i] = target.GetTargetImage(i);

	SetInput(images, viewConfig);
}

const ComputeRenderTarget& IPostProcessPass::GetOutput() const {
	return resolveRenderTarget;
}

void IPostProcessPass::SetupDefaultMaterialInstances() {
	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateStorage_Default();
	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> images{};

	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = resolveRenderTarget.GetTargetImage(i)->GetView(viewConfig);

	if (postProcessingMaterialInstance.HasValue())
		postProcessingMaterialInstance->GetSlot("texture")->SetStorageImages("finalImage", images);
}

void IPostProcessPass::UpdateMaterialInstance() {
	postProcessingMaterialInstance->GetSlot("texture")->FlushUpdate();
}
