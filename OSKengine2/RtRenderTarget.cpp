#include "RtRenderTarget.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "Material.h"
#include "MaterialInstance.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void RtRenderTarget::Create(const Vector2ui& targetSize, RenderTargetAttachmentInfo attachmentInfo) {
	attachmentInfo.usage |= GpuImageUsage::COMPUTE | GpuImageUsage::SAMPLED;
	attachment = RenderTargetAttachment::Create(attachmentInfo, targetSize);

	fullscreenSpriteMaterialInstance = Engine::GetRenderer()->GetFullscreenRenderingMaterial()->CreateInstance().GetPointer();
	SetupSpriteMaterial();
}

void RtRenderTarget::Resize(const Vector2ui& targetSize) {
	attachment.Resize(targetSize);
	SetupSpriteMaterial();
}

GpuImage* RtRenderTarget::GetTargetImage(TSize index) const {
	return attachment.GetImage(index);
}

Vector2ui RtRenderTarget::GetSize() const {
	return GetTargetImage(0)->GetSize2D();
}

MaterialInstance* RtRenderTarget::GetFullscreenSpriteMaterialInstance() const {
	return fullscreenSpriteMaterialInstance.GetPointer();
}

IMaterialSlot* RtRenderTarget::GetFullscreenSpriteMaterialSlot() const {
	return fullscreenSpriteMaterialInstance->GetSlot("texture");
}

void RtRenderTarget::SetupSpriteMaterial() {
	const GpuImageViewConfig view = GpuImageViewConfig::CreateSampled_MipLevelRanged(0, 0);

	const IGpuImageView* images[NUM_RESOURCES_IN_FLIGHT]{};
	for (TIndex i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++)
		images[i] = GetTargetImage(i)->GetView(view);

	fullscreenSpriteMaterialInstance->GetSlot("texture")->SetGpuImages("spriteTexture", images);
	fullscreenSpriteMaterialInstance->GetSlot("texture")->FlushUpdate();
}
