#include "IGpuImageView.h"

#include "Assert.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IGpuImageView::IGpuImageView(SampledChannel channel, SampledArrayType arrayType, TSize baseArrayLevel, TSize layerCount, ViewUsage usage) 
	: channel(channel), arrayType(arrayType), baseArrayLevel(baseArrayLevel), layerCount(layerCount), usage(usage) {

	if (arrayType == SampledArrayType::SINGLE_LAYER)
		OSK_ASSERT(layerCount == 1, "Se ha intentado crear un image view con arrayType == SampledArrayType::SINGLE_LAYER, pero layerCount no es 1.");
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

bool IGpuImageView::operator==(const IGpuImageView& other) {
	return this->channel == other.channel
		&& this->arrayType == other.arrayType
		&& this->baseArrayLevel == other.baseArrayLevel
		&& this->layerCount == other.layerCount
		&& this->usage == other.usage;
}
