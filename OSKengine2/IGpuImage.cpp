#include "IGpuImage.h"

#include "IGpuMemoryBlock.h"
#include "IGpuMemorySubblock.h"
#include <glm/glm.hpp>
#include "OSKengine.h"
#include "IGpuMemoryAllocator.h"
#include "Logger.h"
#include "IRenderer.h"
#include "GpuImageLayout.h"
#include "Format.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuImage::GpuImage(const Vector3ui& size, GpuImageDimension dimension, GpuImageUsage usage, TSize numLayers, Format format, TSize numSamples, GpuImageSamplerDesc samplerDesc)
	: format(format), dimension(dimension), usage(usage), numLayers(numLayers), currentLayout(GpuImageLayout::UNDEFINED), size(size), numSamples(numSamples), samplerDesc(samplerDesc) {
	
	switch (samplerDesc.mipMapMode) {
	case GpuImageMipmapMode::AUTO:
		mipLevels = GetMipLevels(size.X, size.Y);
		break;
	case GpuImageMipmapMode::CUSTOM:
		mipLevels = samplerDesc.maxMipLevel;
		break;
	case GpuImageMipmapMode::NONE:
		mipLevels = 1;
		break;
	}

	_SetPhysicalSize(size);
	_InitDefaultLayout();
}

GpuImage::~GpuImage() {
	if (buffer)
		block->RemoveSubblock(buffer);

	Engine::GetRenderer()->GetAllocator()->RemoveMemoryBlock(block);
}

void GpuImage::SetBlock(OwnedPtr<IGpuMemoryBlock> block) {
	this->block = block.GetPointer();
	buffer = block->GetNextMemorySubblock(block->GetAllocatedSize(), 0);
}

void GpuImage::_SetPhysicalSize(const Vector3ui& size) {
	physicalSize = size;
}

Vector3ui GpuImage::GetSize3D() const {
	return size;
}

Vector2ui GpuImage::GetSize2D() const {
	return { size.X, size.Y };
}

TSize GpuImage::GetSize1D() const {
	return size.X;
}

Vector2ui GpuImage::GetMipLevelSize2D(TIndex mipLevel) const {
	Vector2ui size = GetSize2D();

	for (TIndex i = 0; i < mipLevel; i++)
		size /= 2;

	return size;
}

Vector3ui GpuImage::GetPhysicalSize() const {
	return physicalSize;
}

GpuImageDimension GpuImage::GetDimension() const {
	return dimension;
}

GpuImageUsage GpuImage::GetUsage() const {
	return usage;
}

TSize GpuImage::GetNumLayers() const {
	return numLayers;
}

GpuImageSamplerDesc GpuImage::GetImageSampler() const {
	return samplerDesc;
}

Format GpuImage::GetFormat() const {
	return format;
}

IGpuMemoryBlock* GpuImage::GetMemory() const {
	return block;
}

IGpuMemorySubblock* GpuImage::GetBuffer() const {
	return buffer;
}

unsigned int GpuImage::GetMipLevels() const {
	return mipLevels;
}

TSize GpuImage::GetMipLevels(uint32_t sizeX, uint32_t sizeY) {
	return static_cast<TSize>(glm::floor(glm::log2(glm::max((float)sizeX, (float)sizeY)))) + 1;
}

TSize GpuImage::GetNumberOfBytes() const {
	return size.X * size.Y * size.Z * GetFormatNumberOfBytes(GetFormat());
}

TSize GpuImage::GetPhysicalNumberOfBytes() const {
	return physicalSize.X * physicalSize.Y * physicalSize.Z * GetFormatNumberOfBytes(GetFormat());
}

TSize GpuImage::GetNumSamples() const {
	return numSamples;
}

IGpuImageView* GpuImage::GetView(const GpuImageViewConfig& viewConfig) const {
	if (views.HasValue(viewConfig))
		return views.Get(viewConfig).GetPointer();

	OwnedPtr<IGpuImageView> view = CreateView(viewConfig);
	views.Insert(viewConfig, view.GetPointer());

	return view.GetPointer();
}
void GpuImage::SetCurrentBarrier(const GpuBarrierInfo& barrier) {
	currentBarrier = barrier;
}

const GpuBarrierInfo& GpuImage::GetCurrentBarrier() const {
	return currentBarrier;
}

void GpuImage::_InitDefaultLayout() {
	const TSize arrayLevelCount = numLayers;
	const TSize mipLevelCount = mipLevels;

	for (ArrayLevelIndex a = 0; a < arrayLevelCount; a++) {
		layouts.Insert(a, {});
		auto& arrayLevel = layouts.Get(a);

		for (MipLevelIndex m = 0; m < mipLevelCount; m++)
			arrayLevel.Insert(m, GpuImageLayout::UNDEFINED);
	}
}

void GpuImage::_SetLayout(ArrayLevelIndex baseArrayLevel, TSize arrayLevelCount, MipLevelIndex baseMipLevel, TSize mipLevelCount, GpuImageLayout layout) {
	arrayLevelCount = glm::min(arrayLevelCount, numLayers);
	mipLevelCount = glm::min(mipLevelCount, mipLevels);
	
	for (ArrayLevelIndex a = baseArrayLevel; a < baseArrayLevel + arrayLevelCount; a++) {
		auto& arrayLevel = layouts.Get(a);

		for (MipLevelIndex m = baseMipLevel; m < baseMipLevel + mipLevelCount; m++)
			arrayLevel.Insert(m, layout);
	}
}
GpuImageLayout GpuImage::_GetLayout(ArrayLevelIndex arrayLevel, MipLevelIndex mipLevel) const {
	return layouts.Get(arrayLevel).Get(mipLevel);
}
