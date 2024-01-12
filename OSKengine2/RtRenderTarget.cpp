#include "RtRenderTarget.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "Material.h"
#include "MaterialInstance.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void RtRenderTarget::Create(const Vector2ui& targetSize, RenderTargetAttachmentInfo attachmentInfo) {
	attachmentInfo.usage |= GpuImageUsage::COMPUTE | GpuImageUsage::SAMPLED;
	m_attachment = RenderTargetAttachment::Create(attachmentInfo, targetSize);

	m_fullscreenSpriteMaterialInstance = Engine::GetRenderer()->GetFullscreenRenderingMaterial()->CreateInstance().GetPointer();
	SetupSpriteMaterial();
}

void RtRenderTarget::Resize(const Vector2ui& targetSize) {
	m_attachment.Resize(targetSize);
	SetupSpriteMaterial();
}

GpuImage* RtRenderTarget::GetTargetImage() {
	return m_attachment.GetImage();
}

const GpuImage* RtRenderTarget::GetTargetImage() const {
	return m_attachment.GetImage();
}

Vector2ui RtRenderTarget::GetSize() const {
	return GetTargetImage()->GetSize2D();
}

MaterialInstance* RtRenderTarget::GetFullscreenSpriteMaterialInstance() const {
	return m_fullscreenSpriteMaterialInstance.GetPointer();
}

IMaterialSlot* RtRenderTarget::GetFullscreenSpriteMaterialSlot() const {
	return m_fullscreenSpriteMaterialInstance->GetSlot("texture");
}

void RtRenderTarget::SetupSpriteMaterial() {
	const GpuImageViewConfig view = GpuImageViewConfig::CreateSampled_MipLevelRanged(0, 0);

	m_fullscreenSpriteMaterialInstance->GetSlot("texture")->SetGpuImage("spriteTexture", GetTargetImage()->GetView(view));
	m_fullscreenSpriteMaterialInstance->GetSlot("texture")->FlushUpdate();
}
