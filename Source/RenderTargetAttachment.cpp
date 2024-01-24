#include "RenderTargetAttachment.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "GpuMemoryTypes.h"

#include "InvalidArgumentException.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


RenderTargetAttachment RenderTargetAttachment::Create(const RenderTargetAttachmentInfo& info, const Vector2ui& resolution) {
	RenderTargetAttachment output{};
	output.Initialize(info, resolution);

	return output;
}

void RenderTargetAttachment::Initialize(const RenderTargetAttachmentInfo& info, const Vector2ui& resolution) {
	m_info = info;
	
	GpuImageCreateInfo imageInfo = GpuImageCreateInfo::CreateDefault2D(resolution, info.format, info.usage);
	imageInfo.samplerDesc = info.sampler;

	m_image = Engine::GetRenderer()->GetAllocator()->CreateImage(imageInfo).GetPointer();
	m_image->SetDebugName(info.name);
}

void RenderTargetAttachment::Resize(const Vector2ui& resolution) {
	Initialize(m_info, resolution);
}

const RenderTargetAttachmentInfo& RenderTargetAttachment::GetInfo() const {
	return m_info;
}

GpuImage* RenderTargetAttachment::GetImage() {
	return m_image.GetPointer();
}

const GpuImage* RenderTargetAttachment::GetImage() const {
	return m_image.GetPointer();
}
