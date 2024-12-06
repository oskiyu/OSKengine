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
		m_colorAttachments.Insert(RenderTargetAttachment::Create(attachmentInfo, targetSize));
	}

	depthInfo.usage |= GpuImageUsage::DEPTH;

	if (FormatSupportsStencil(depthInfo.format)) {
		depthInfo.usage |= GpuImageUsage::STENCIL;
	}

	m_depthAttachment.Initialize(depthInfo, targetSize);

	m_targetType = RenderpassType::INTERMEDIATE;

	m_fullscreenSpriteMaterialInstance = Engine::GetRenderer()->GetFullscreenRenderingMaterial()->CreateInstance().GetPointer();
	SetupSpriteMaterial();
}

void RenderTarget::CreateAsFinal(const Vector2ui& targetSize, RenderTargetAttachmentInfo colorInfo, RenderTargetAttachmentInfo depthInfo) {
	colorInfo.usage |= GpuImageUsage::COLOR | GpuImageUsage::SAMPLED;
	m_colorAttachments.Insert(RenderTargetAttachment::Create(colorInfo, targetSize));
	
	depthInfo.usage |= GpuImageUsage::DEPTH | GpuImageUsage::STENCIL;
	m_depthAttachment.Initialize(depthInfo, targetSize);

	m_targetType = RenderpassType::FINAL;

	m_fullscreenSpriteMaterialInstance = Engine::GetRenderer()->GetFullscreenRenderingMaterial()->CreateInstance().GetPointer();
	SetupSpriteMaterial();
}

void RenderTarget::SetupSpriteMaterial() {
	const GpuImageViewConfig view = GpuImageViewConfig::CreateSampled_MipLevelRanged(0, 0);

	m_fullscreenSpriteMaterialInstance->GetSlot("texture")->SetGpuImage("spriteTexture", *GetMainColorImage()->GetView(view), GpuImageSamplerDesc::CreateDefault_NoMipMap());
	m_fullscreenSpriteMaterialInstance->GetSlot("texture")->FlushUpdate();
}

void RenderTarget::Resize(const Vector2ui& targetSize) {
	for (auto& i : m_colorAttachments)
		i.Resize(targetSize);

	m_depthAttachment.Resize(targetSize);

	SetupSpriteMaterial();
}

GpuImage* RenderTarget::GetColorImage(UIndex32 colorImageIndex) {
	OSK_ASSERT(colorImageIndex < m_colorAttachments.GetSize(),
		InvalidArgumentException(std::format("Sólo hay {}  imágenes de color.", m_colorAttachments.GetSize())));

	return m_colorAttachments[colorImageIndex].GetImage();
}

const GpuImage* RenderTarget::GetColorImage(UIndex32 colorImageIndex) const {
	OSK_ASSERT(colorImageIndex < m_colorAttachments.GetSize(),
		InvalidArgumentException(std::format("Sólo hay {}  imágenes de color.", m_colorAttachments.GetSize())));

	return m_colorAttachments[colorImageIndex].GetImage();
}

GpuImage* RenderTarget::GetMainColorImage() {
	return GetColorImage(0);
}

const GpuImage* RenderTarget::GetMainColorImage() const {
	return GetColorImage(0);
}

GpuImage* RenderTarget::GetDepthImage() {
	return m_depthAttachment.GetImage();
}

const GpuImage* RenderTarget::GetDepthImage() const {
	return m_depthAttachment.GetImage();
}

Vector2ui RenderTarget::GetSize() const {
	return m_depthAttachment.GetImage()->GetSize2D();
}

RenderpassType RenderTarget::GetRenderTargetType() const {
	return m_targetType;
}

USize32 RenderTarget::GetNumColorTargets() const {
	return static_cast<USize32>(m_colorAttachments.GetSize());
}

MaterialInstance* RenderTarget::GetFullscreenSpriteMaterialInstance() {
	return m_fullscreenSpriteMaterialInstance.GetPointer();
}

IMaterialSlot* RenderTarget::GetFullscreenSpriteMaterialSlot() {
	return m_fullscreenSpriteMaterialInstance->GetSlot("texture");
}
