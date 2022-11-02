#include "RenderTargetAttachment.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "GpuMemoryTypes.h"

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
	
	for (TSize i = 0; i < NUM_RESOURCES_IN_FLIGHT; i++) {
		images[i] = Engine::GetRenderer()->GetAllocator()->CreateImage(
			{ resolution.X, resolution.Y, 1 }, GpuImageDimension::d2D, 1, info.format,
			info.usage, GpuSharedMemoryType::GPU_ONLY, 1, info.sampler).GetPointer();
		
		images[i]->SetDebugName(info.name + "[" + std::to_string(i) + "]");
	}
}

void RenderTargetAttachment::Resize(const Vector2ui& resolution) {
	Initialize(info, resolution);
}

const RenderTargetAttachmentInfo& RenderTargetAttachment::GetInfo() const {
	return info;
}

GpuImage* RenderTargetAttachment::GetImage(TIndex resourceIndex) const {
	OSK_ASSERT(resourceIndex < NUM_RESOURCES_IN_FLIGHT, "No existe imagen en el índice " + std::to_string(resourceIndex));
	return images[resourceIndex].GetPointer();
}
