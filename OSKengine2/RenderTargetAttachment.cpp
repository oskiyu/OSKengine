#include "RenderTargetAttachment.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "GpuMemoryTypes.h"

#include "InvalidArgumentException.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

RenderTargetAttachment::RenderTargetAttachment() {

}

RenderTargetAttachment RenderTargetAttachment::Create(const RenderTargetAttachmentInfo& info, const Vector2ui& resolution) {
	RenderTargetAttachment output{};
	output.Initialize(info, resolution);

	return output;
}

void RenderTargetAttachment::Initialize(const RenderTargetAttachmentInfo& info, const Vector2ui& resolution) {
	this->info = info;
	
	for (UIndex32 i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		GpuImageCreateInfo imageInfo = GpuImageCreateInfo::CreateDefault2D(resolution, info.format, info.usage);
		imageInfo.samplerDesc = info.sampler;

		images[i] = Engine::GetRenderer()->GetAllocator()->CreateImage(imageInfo).GetPointer();
		
		images[i]->SetDebugName(info.name + "[" + std::to_string(i) + "]");
	}
}

void RenderTargetAttachment::Resize(const Vector2ui& resolution) {
	Initialize(info, resolution);
}

const RenderTargetAttachmentInfo& RenderTargetAttachment::GetInfo() const {
	return info;
}

GpuImage* RenderTargetAttachment::GetImage(UIndex32 resourceIndex) const {
	OSK_ASSERT(resourceIndex < NUM_RESOURCES_IN_FLIGHT, 
		InvalidArgumentException("No existe imagen en el índice " + std::to_string(resourceIndex)));
	return images[resourceIndex].GetPointer();
}
