#include "RenderTarget.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "Format.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"
#include "GpuMemoryTypes.h"
#include "Material.h"

#include "InvalidArgumentException.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void RenderTarget::Create(const Vector2ui& targetSize, DynamicArray<RenderTargetAttachmentInfo> colorInfos, RenderTargetAttachmentInfo depthInfo) {
	OSK_ASSERT(colorInfos.GetSize() > 0, InvalidArgumentException("Debe haber al menos un color attachment"));

	for (auto& attachmentInfo : colorInfos) {
		attachmentInfo.usage |= GpuImageUsage::COLOR | GpuImageUsage::SAMPLED;
		colorAttachments.Insert(RenderTargetAttachment::Create(attachmentInfo, targetSize));
	}
	depthInfo.usage |= GpuImageUsage::DEPTH;
	if (FormatSupportsStencil(depthInfo.format))
		depthInfo.usage |= GpuImageUsage::STENCIL;

	depthAttachment.Initialize(depthInfo, targetSize);

	targetType = RenderpassType::INTERMEDIATE;

	fullscreenSpriteMaterialInstance = Engine::GetRenderer()
		->GetFullscreenRenderingMaterial()->CreateInstance().GetPointer();
	SetupSpriteMaterial();
}

void RenderTarget::CreateAsFinal(const Vector2ui& targetSize, RenderTargetAttachmentInfo colorInfo, RenderTargetAttachmentInfo depthInfo) {
	colorInfo.usage |= GpuImageUsage::COLOR | GpuImageUsage::SAMPLED;
	colorAttachments.Insert(RenderTargetAttachment::Create(colorInfo, targetSize));
	
	depthInfo.usage |= GpuImageUsage::DEPTH | GpuImageUsage::STENCIL;
	depthAttachment.Initialize(depthInfo, targetSize);

	targetType = RenderpassType::FINAL;

	fullscreenSpriteMaterialInstance = Engine::GetRenderer()
		->GetFullscreenRenderingMaterial()->CreateInstance().GetPointer();
	SetupSpriteMaterial();
}

void RenderTarget::SetupSpriteMaterial() {
	const GpuImageViewConfig view = GpuImageViewConfig::CreateSampled_MipLevelRanged(0, 0);

	std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> images{};
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = GetColorImage(0, i)->GetView(view);

	fullscreenSpriteMaterialInstance->GetSlot("texture")->SetGpuImages("spriteTexture", images);
	fullscreenSpriteMaterialInstance->GetSlot("texture")->FlushUpdate();
}

void RenderTarget::Resize(const Vector2ui& targetSize) {
	for (auto& i : colorAttachments)
		i.Resize(targetSize);
	depthAttachment.Resize(targetSize);

	SetupSpriteMaterial();
}

GpuImage* RenderTarget::GetColorImage(UIndex32 colorImageIndex, UIndex32 resourceIndex) const {
	OSK_ASSERT(resourceIndex < NUM_RESOURCES_IN_FLIGHT,
		InvalidArgumentException(std::format("El índice de la imagen debe estar entre 0 y {}", NUM_RESOURCES_IN_FLIGHT - 1)));
	OSK_ASSERT(colorImageIndex < colorAttachments.GetSize(),
		InvalidArgumentException(std::format("Sólo hay {}  imágenes de color.", colorAttachments.GetSize())));

	return colorAttachments[colorImageIndex].GetImage(resourceIndex);
}

GpuImage* RenderTarget::GetMainColorImage(UIndex32 resourceIndex) const {
	return GetColorImage(0, resourceIndex);
}

GpuImage* RenderTarget::GetDepthImage(UIndex32 index) const {
	OSK_ASSERT(index < NUM_RESOURCES_IN_FLIGHT, 
		InvalidArgumentException(std::format("El índice de la imagen debe estar entre 0 y {}", NUM_RESOURCES_IN_FLIGHT - 1)));

	return depthAttachment.GetImage(index);
}

Vector2ui RenderTarget::GetSize() const {
	return depthAttachment.GetImage(0)->GetSize2D();
}

RenderpassType RenderTarget::GetRenderTargetType() const {
	return targetType;
}

USize32 RenderTarget::GetNumColorTargets() const {
	return static_cast<USize32>(colorAttachments.GetSize());
}

MaterialInstance* RenderTarget::GetFullscreenSpriteMaterialInstance() {
	return fullscreenSpriteMaterialInstance.GetPointer();
}

IMaterialSlot* RenderTarget::GetFullscreenSpriteMaterialSlot() {
	return fullscreenSpriteMaterialInstance->GetSlot("texture");
}
