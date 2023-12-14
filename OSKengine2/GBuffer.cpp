#include "GBuffer.h"

#include "Format.h"
#include "ICommandList.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void GBuffer::Create(const Vector2ui& resolution, GpuImageSamplerDesc sampler, GpuImageUsage usage) {
	sampler.mipMapMode = GpuImageMipmapMode::NONE;

	DynamicArray<RenderTargetAttachmentInfo> colorAttachments{};
	colorAttachments.Insert({ .format = Format::RGBA8_UNORM,   .usage = usage, .sampler = sampler, .name = "GBuffer Color" });
	colorAttachments.Insert({ .format = Format::RGB10A2_UNORM, .usage = usage, .sampler = sampler, .name = "GBuffer Normal" });
	colorAttachments.Insert({ .format = Format::RG16_SFLOAT, .usage = usage, .sampler = sampler, .name = "GBuffer MetallicRoughness" });
	colorAttachments.Insert({ .format = Format::RG16_SFLOAT, .usage = usage, .sampler = sampler, .name = "GBuffer Motion" });
	colorAttachments.Insert({ .format = Format::RGBA8_UNORM,   .usage = usage, .sampler = sampler, .name = "GBuffer Emission" });
	
	RenderTargetAttachmentInfo depthInfo{};
	depthInfo.format = Format::D32_SFLOAT;
	depthInfo.usage = GpuImageUsage::DEPTH | GpuImageUsage::SAMPLED;
	depthInfo.name = "GBuffer Depth";
	depthInfo.sampler = sampler;

	renderTarget.Create(
		resolution, 
		colorAttachments, 
		depthInfo);
}

void GBuffer::Resize(const Vector2ui& resolution) {
	renderTarget.Resize(resolution);
}

GpuImage* GBuffer::GetImage(USize32 frameIndex, Target targetType) {
	return targetType == Target::DEPTH
		? renderTarget.GetDepthImage(frameIndex)
		: renderTarget.GetColorImage(static_cast<UIndex32>(targetType), frameIndex);
}

const GpuImage* GBuffer::GetImage(UIndex32 frameIndex, Target targetType) const {
	return targetType == Target::DEPTH
		? renderTarget.GetDepthImage(frameIndex)
		: renderTarget.GetColorImage(static_cast<UIndex32>(targetType), frameIndex);
}

void GBuffer::BeginRenderpass(ICommandList* cmdList, Color color) {
	cmdList->BeginGraphicsRenderpass(&renderTarget, color);
}

const std::array<GBuffer::Target, 5> GBuffer::ColorTargetTypes = { Target::COLOR, Target::NORMAL, Target::METALLIC_ROUGHNESS, Target::MOTION, Target::EMISSIVE };
