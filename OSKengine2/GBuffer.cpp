#include "GBuffer.h"

#include "Format.h"
#include "ICommandList.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void GBuffer::Create(const Vector2ui& resolution, GpuImageSamplerDesc sampler, GpuImageUsage usage) {
	DynamicArray<RenderTargetAttachmentInfo> colorAttachments{};
	colorAttachments.Insert({ .format = Format::RGBA16_SFLOAT, .usage = usage, .sampler = sampler, .name = "GBuffer Position" });
	colorAttachments.Insert({ .format = Format::RGBA8_UNORM,   .usage = usage, .sampler = sampler, .name = "GBuffer Color" });
	colorAttachments.Insert({ .format = Format::RGBA16_SFLOAT, .usage = usage, .sampler = sampler, .name = "GBuffer Normal" });
	colorAttachments.Insert({ .format = Format::RGBA16_SFLOAT, .usage = usage, .sampler = sampler, .name = "GBuffer Motion" });
	
	renderTarget.Create(resolution, colorAttachments, { .format = Format::D32S8_SFLOAT_SUINT, .usage = GpuImageUsage::DEPTH_STENCIL, .sampler = sampler });
}

void GBuffer::Resize(const Vector2ui& resolution) {
	renderTarget.Resize(resolution);
}

GpuImage* GBuffer::GetImage(TIndex frameIndex, Target targetType) {
	return targetType == Target::DEPTH
		? renderTarget.GetDepthImage(frameIndex)
		: renderTarget.GetColorImage(static_cast<TIndex>(targetType), frameIndex);
}

void GBuffer::BeginRenderpass(ICommandList* cmdList, Color color) {
	cmdList->BeginGraphicsRenderpass(&renderTarget, color);
}

const GBuffer::Target GBuffer::ColorTargetTypes[4] = { Target::POSITION, Target::COLOR, Target::NORMAL, Target::MOTION };
