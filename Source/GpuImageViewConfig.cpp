#include "GpuImageViewConfig.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuImageViewConfig GpuImageViewConfig::CreateSampled_Default() {
	GpuImageViewConfig output{};

	output.channel = SampledChannel::COLOR;
	output.arrayType = SampledArrayType::SINGLE_LAYER;
	output.baseArrayLevel = 0;
	output.arrayLevelCount = 1;
	output.mipMapMode = MipMapMode::ALL_LEVELS;
	output.usage = ViewUsage::SAMPLED;

	return output;
}

GpuImageViewConfig GpuImageViewConfig::CreateSampled_SingleMipLevel(UIndex32 mipLevel) {
	return CreateSampled_MipLevelRanged(mipLevel, mipLevel);
}

GpuImageViewConfig GpuImageViewConfig::CreateSampled_MipLevelRanged(UIndex32 baseMipLevel, UIndex32 topMipLevel) {
	GpuImageViewConfig output{};

	output.channel = SampledChannel::COLOR;
	output.arrayType = SampledArrayType::SINGLE_LAYER;
	output.baseArrayLevel = 0;
	output.arrayLevelCount = 1;
	output.mipMapMode = MipMapMode::LEVEL_RANGE;
	output.baseMipLevel = baseMipLevel;
	output.topMipLevel = topMipLevel;
	output.usage = ViewUsage::SAMPLED;

	return output;
}

GpuImageViewConfig GpuImageViewConfig::CreateSampled_Array(UIndex32 baseLayer, USize32 layerCount) {
	GpuImageViewConfig output{};

	output.channel = SampledChannel::COLOR;
	output.arrayType = SampledArrayType::ARRAY;
	output.baseArrayLevel = baseLayer;
	output.arrayLevelCount = layerCount;
	output.mipMapMode = MipMapMode::ALL_LEVELS;
	output.usage = ViewUsage::SAMPLED;

	return output;
}

GpuImageViewConfig GpuImageViewConfig::CreateSampled_Cubemap() {
	GpuImageViewConfig output{};

	output.channel = SampledChannel::COLOR;
	output.arrayType = SampledArrayType::SINGLE_LAYER;
	output.baseArrayLevel = 0;
	output.arrayLevelCount = 6;
	output.mipMapMode = MipMapMode::ALL_LEVELS;
	output.usage = ViewUsage::SAMPLED;

	return output;
}


GpuImageViewConfig GpuImageViewConfig::CreateStorage_Default() {
	GpuImageViewConfig output{};

	output.channel = SampledChannel::COLOR;
	output.arrayType = SampledArrayType::SINGLE_LAYER;
	output.baseArrayLevel = 0;
	output.arrayLevelCount = 1;
	output.mipMapMode = MipMapMode::LEVEL_RANGE;
	output.baseMipLevel = 0;
	output.topMipLevel = 0;
	output.usage = ViewUsage::STORAGE;

	return output;
}


GpuImageViewConfig GpuImageViewConfig::CreateTarget_Color() {
	GpuImageViewConfig output{};

	output.channel = SampledChannel::COLOR;
	output.arrayType = SampledArrayType::SINGLE_LAYER;
	output.baseArrayLevel = 0;
	output.arrayLevelCount = 1;
	output.mipMapMode = MipMapMode::LEVEL_RANGE;
	output.baseMipLevel = 0;
	output.topMipLevel = 0;
	output.usage = ViewUsage::COLOR_TARGET;

	return output;
}

GpuImageViewConfig GpuImageViewConfig::CreateTarget_Depth() {
	GpuImageViewConfig output{};

	output.channel = SampledChannel::DEPTH;
	output.arrayType = SampledArrayType::SINGLE_LAYER;
	output.baseArrayLevel = 0;
	output.arrayLevelCount = 1;
	output.mipMapMode = MipMapMode::LEVEL_RANGE;
	output.baseMipLevel = 0;
	output.topMipLevel = 0;
	output.usage = ViewUsage::DEPTH_ONLY_TARGET;

	return output;
}

GpuImageViewConfig GpuImageViewConfig::CreateTarget_Stencil() {
	GpuImageViewConfig output{};

	output.channel = SampledChannel::STENCIL;
	output.arrayType = SampledArrayType::SINGLE_LAYER;
	output.baseArrayLevel = 0;
	output.arrayLevelCount = 1;
	output.mipMapMode = MipMapMode::LEVEL_RANGE;
	output.baseMipLevel = 0;
	output.topMipLevel = 0;
	output.usage = ViewUsage::DEPTH_STENCIL_TARGET;

	return output;
}

GpuImageViewConfig GpuImageViewConfig::CreateTarget_DepthStencil() {
	GpuImageViewConfig output{};

	output.channel = SampledChannel::DEPTH | SampledChannel::STENCIL;
	output.arrayType = SampledArrayType::SINGLE_LAYER;
	output.baseArrayLevel = 0;
	output.arrayLevelCount = 1;
	output.mipMapMode = MipMapMode::LEVEL_RANGE;
	output.baseMipLevel = 0;
	output.topMipLevel = 0;
	output.usage = ViewUsage::DEPTH_STENCIL_TARGET;

	return output;
}
