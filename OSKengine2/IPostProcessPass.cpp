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
	info.usage = GpuImageUsage::COMPUTE | GpuImageUsage::SAMPLED;
	info.sampler = GpuImageSamplerDesc::CreateDefault();
	resolveRenderTarget.Create(size, info);
	
	SetupDefaultMaterialInstances();
}

void IPostProcessPass::Resize(const Vector2ui& size) {
	resolveRenderTarget.Resize(size);
	
	SetupDefaultMaterialInstances();
}

void IPostProcessPass::SetInput(GpuImage* images[3], InputType type) {
	const GpuImage* imgs[3]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		imgs[i] = images[i];
		inputImages[i] = images[i];
	}

	if (postProcessingMaterialInstance.HasValue()) {
		if (type == InputType::STORAGE_IMAGE)
			postProcessingMaterialInstance->GetSlot("texture")->SetStorageImages("sceneImage", imgs);
		else
			postProcessingMaterialInstance->GetSlot("texture")->SetGpuImages("sceneImage", imgs);
	}
}

void IPostProcessPass::SetInput(const RenderTarget& target, InputType type) {
	GpuImage* images[3]{};
	for (TSize i = 0; i < 3; i++)
		images[i] = target.GetMainColorImage(i);

	SetInput(images, type);
}

void IPostProcessPass::SetInput(const RtRenderTarget& target, InputType type) {
	GpuImage* images[3]{};
	for (TSize i = 0; i < 3; i++)
		images[i] = target.GetTargetImage(i);

	SetInput(images, type);
}

const ComputeRenderTarget& IPostProcessPass::GetOutput() const {
	return resolveRenderTarget;
}

void IPostProcessPass::SetupDefaultMaterialInstances() {
	const GpuImage* images[NUM_RESOURCES_IN_FLIGHT]{};
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = resolveRenderTarget.GetTargetImage(i);

	if (postProcessingMaterialInstance.HasValue())
		postProcessingMaterialInstance->GetSlot("texture")->SetStorageImages("finalImage", images);
}

void IPostProcessPass::UpdateMaterialInstance() {
	postProcessingMaterialInstance->GetSlot("texture")->FlushUpdate();
}
