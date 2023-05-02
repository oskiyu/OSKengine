#include "IGpuImageView.h"

#include "Assert.h"
#include "IGpuImage.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IGpuImageView::IGpuImageView(
	const GpuImage& originalImage,
	const GpuImageViewConfig& config)

	: originalImage(&originalImage), config(config) {
	size = originalImage.GetSize3D();
}

SampledChannel IGpuImageView::GetChannel() const {
	return config.channel;
}

SampledArrayType IGpuImageView::GetArrayType() const {
	return config.arrayType;
}

TSize IGpuImageView::GetBaseArrayLevel() const {
	return config.baseArrayLevel;
}

TSize IGpuImageView::GetLayerCount() const {
	return config.arrayLevelCount;
}

ViewUsage IGpuImageView::GetViewUsage() const {
	return config.usage;
}
