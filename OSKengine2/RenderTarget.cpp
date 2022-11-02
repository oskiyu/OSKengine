#include "RenderTarget.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "Format.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"
#include "GpuMemoryTypes.h"
#include "Material.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void RenderTarget::Create(const Vector2ui& targetSize, DynamicArray<RenderTargetAttachmentInfo> colorInfos, RenderTargetAttachmentInfo depthInfo) {
	OSK_ASSERT(colorInfos.GetSize() > 0, "Debe haber al menos un attachment");

	for (auto& attachmentInfo : colorInfos) {
		attachmentInfo.usage |= GpuImageUsage::COLOR | GpuImageUsage::SAMPLED;
		colorAttachments.Insert(RenderTargetAttachment::Create(attachmentInfo, targetSize));
	}
	depthInfo.usage |= GpuImageUsage::DEPTH_STENCIL;
	depthAttachment.Initialize(depthInfo, targetSize);

	targetType = RenderpassType::INTERMEDIATE;

	fullscreenSpriteMaterialInstance = Engine::GetRenderer()->GetFullscreenRenderingMaterial()->CreateInstance().GetPointer();
	SetupSpriteMaterial();
}

void RenderTarget::CreateAsFinal(const Vector2ui& targetSize, RenderTargetAttachmentInfo colorInfo, RenderTargetAttachmentInfo depthInfo) {
	colorInfo.usage |= GpuImageUsage::COLOR | GpuImageUsage::SAMPLED;
	colorAttachments.Insert(RenderTargetAttachment::Create(colorInfo, targetSize));
	
	depthInfo.usage |= GpuImageUsage::DEPTH_STENCIL;
	depthAttachment.Initialize(depthInfo, targetSize);

	targetType = RenderpassType::FINAL;

	fullscreenSpriteMaterialInstance = Engine::GetRenderer()->GetFullscreenRenderingMaterial()->CreateInstance().GetPointer();
	SetupSpriteMaterial();
}

void RenderTarget::SetupSpriteMaterial() {
	const GpuImage* images[NUM_RESOURCES_IN_FLIGHT]{};
	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = GetColorImage(0, i);

	fullscreenSpriteMaterialInstance->GetSlot("texture")->SetGpuImages("spriteTexture", images);
	fullscreenSpriteMaterialInstance->GetSlot("texture")->FlushUpdate();
}

void RenderTarget::Resize(const Vector2ui& targetSize) {
	for (auto& i : colorAttachments)
		i.Resize(targetSize);
	depthAttachment.Resize(targetSize);

	SetupSpriteMaterial();
}

GpuImage* RenderTarget::GetColorImage(TIndex colorImageIndex, TIndex resourceIndex) const {
	OSK_ASSERT(resourceIndex < NUM_RESOURCES_IN_FLIGHT, "El índice de la imagen debe estar entre 0 y " + std::to_string(NUM_RESOURCES_IN_FLIGHT - 1));
	OSK_ASSERT(colorImageIndex < colorAttachments.GetSize(), "Sólo hay " + std::to_string(colorAttachments.GetSize()) + " imágenes de color.");

	return colorAttachments[colorImageIndex].GetImage(resourceIndex);
}

GpuImage* RenderTarget::GetMainColorImage(TIndex resourceIndex) const {
	return GetColorImage(0, resourceIndex);
}

GpuImage* RenderTarget::GetDepthImage(TIndex index) const {
	OSK_ASSERT(index < NUM_RESOURCES_IN_FLIGHT, "El índice de la imagen debe estar entre 0 y " + std::to_string(NUM_RESOURCES_IN_FLIGHT - 1));

	return depthAttachment.GetImage(index);
}

Vector2ui RenderTarget::GetSize() const {
	return { depthAttachment.GetImage(0)->GetSize().X, depthAttachment.GetImage(0)->GetSize().Y };
}

RenderpassType RenderTarget::GetRenderTargetType() const {
	return targetType;
}

TSize RenderTarget::GetNumColorTargets() const {
	return colorAttachments.GetSize();
}

MaterialInstance* RenderTarget::GetFullscreenSpriteMaterialInstance() const {
	return fullscreenSpriteMaterialInstance.GetPointer();
}

IMaterialSlot* RenderTarget::GetFullscreenSpriteMaterialSlot() const {
	return fullscreenSpriteMaterialInstance->GetSlot("texture");
}
