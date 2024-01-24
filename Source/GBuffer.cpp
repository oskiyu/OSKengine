#include "GBuffer.h"

#include "Format.h"
#include "ICommandList.h"
#include "GpuImageLayout.h"

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

GpuImage* GBuffer::GetImage(Target targetType) {
	return targetType == Target::DEPTH
		? renderTarget.GetDepthImage()
		: renderTarget.GetColorImage(static_cast<UIndex32>(targetType));
}

const GpuImage* GBuffer::GetImage(Target targetType) const {
	return targetType == Target::DEPTH
		? renderTarget.GetDepthImage()
		: renderTarget.GetColorImage(static_cast<UIndex32>(targetType));
}

void GBuffer::BindPipelineBarriers(ICommandList* cmdList) {
	for (UIndex64 i = 0; i < renderTarget.GetNumColorTargets(); i++) {
		cmdList->SetGpuImageBarrier(
			renderTarget.GetColorImage(i),
			GpuImageLayout::UNDEFINED,
			GpuImageLayout::COLOR_ATTACHMENT,
			GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
			GpuBarrierInfo(GpuCommandStage::COLOR_ATTACHMENT_OUTPUT, GpuAccessStage::COLOR_ATTACHMENT_WRITE),
			{ .baseLayer = 0,
			.numLayers = 1,
			.baseMipLevel = 0,
			.numMipLevels = ALL_MIP_LEVELS });
	}

	SampledChannel depthChannel = SampledChannel::DEPTH;
	if (FormatSupportsStencil(renderTarget.GetDepthImage()->GetFormat()))
		depthChannel |= SampledChannel::STENCIL;

	cmdList->SetGpuImageBarrier(
		renderTarget.GetDepthImage(),
		GpuImageLayout::UNDEFINED,
		GpuImageLayout::DEPTH_STENCIL_TARGET,
		GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
		GpuBarrierInfo(GpuCommandStage::DEPTH_STENCIL_START, GpuAccessStage::DEPTH_STENCIL_READ | GpuAccessStage::DEPTH_STENCIL_WRITE),
		{ .baseLayer = 0,
			.numLayers = 1,
			.baseMipLevel = 0,
			.numMipLevels = ALL_MIP_LEVELS,
			.channel = depthChannel });
}

void GBuffer::BeginRenderpass(ICommandList* cmdList, Color color, bool autoSync) {
	cmdList->BeginGraphicsRenderpass(&renderTarget, color, autoSync);
}

const std::array<GBuffer::Target, 5> GBuffer::ColorTargetTypes = { Target::COLOR, Target::NORMAL, Target::METALLIC_ROUGHNESS, Target::MOTION, Target::EMISSIVE };
