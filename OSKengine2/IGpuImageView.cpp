#include "IGpuImageView.h"

#include "Assert.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IGpuImageView::IGpuImageView(SampledChannel channel, SampledArrayType arrayType, TSize baseArrayLevel, TSize layerCount, ViewUsage usage) 
	: channel(channel), arrayType(arrayType), baseArrayLevel(baseArrayLevel), layerCount(layerCount), usage(usage) {

}

SampledChannel IGpuImageView::GetChannel() const {
	return channel;
}

SampledArrayType IGpuImageView::GetArrayType() const {
	return arrayType;
}

TSize IGpuImageView::GetBaseArrayLevel() const {
	return baseArrayLevel;
}

TSize IGpuImageView::GetLayerCount() const {
	return layerCount;
}

ViewUsage IGpuImageView::GetViewUsage() const {
	return usage;
}

bool IGpuImageView::operator==(const IGpuImageView& other) const {
	return this->channel == other.channel
		&& this->arrayType == other.arrayType
		&& this->baseArrayLevel == other.baseArrayLevel
		&& this->layerCount == other.layerCount
		&& this->usage == other.usage;
}
