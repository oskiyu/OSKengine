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

GpuImage::GpuImage(const Vector3ui& size, GpuImageDimension dimension, GpuImageUsage usage, USize32 numLayers, Format format, USize32 numSamples, GpuImageSamplerDesc samplerDesc)
	: size(size), samplerDesc(samplerDesc), numSamples(numSamples), format(format), currentLayout(GpuImageLayout::UNDEFINED), dimension(dimension), usage(usage), numLayers(numLayers) {
	
	switch (samplerDesc.mipMapMode) {
	case GpuImageMipmapMode::AUTO:
		mipLevels = GetMipLevels(size.x, size.y);
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
	return { size.x, size.y };
}

USize32 GpuImage::GetSize1D() const {
	return size.x;
}

Vector2ui GpuImage::GetMipLevelSize2D(UIndex32 mipLevel) const {
	Vector2ui size = GetSize2D();

	for (UIndex32 i = 0; i < mipLevel; i++)
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

USize32 GpuImage::GetNumLayers() const {
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

USize32 GpuImage::GetMipLevels(uint32_t sizeX, uint32_t sizeY) {
	return static_cast<USize32>(glm::floor(glm::log2(glm::max((float)sizeX, (float)sizeY)))) + 1;
}

USize64 GpuImage::GetNumberOfBytes() const {
	return size.x * size.y * size.Z * GetFormatNumberOfBytes(GetFormat());
}

USize64 GpuImage::GetPhysicalNumberOfBytes() const {
	return physicalSize.x * physicalSize.y * physicalSize.Z * GetFormatNumberOfBytes(GetFormat());
}

USize32 GpuImage::GetNumSamples() const {
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
	const USize32 arrayLevelCount = numLayers;
	const USize32 mipLevelCount = mipLevels;

	for (ArrayLevelIndex a = 0; a < arrayLevelCount; a++) {
		layouts.Insert(a, {});
		auto& arrayLevel = layouts.Get(a);

		for (MipLevelIndex m = 0; m < mipLevelCount; m++)
			arrayLevel.Insert(m, GpuImageLayout::UNDEFINED);
	}
}

void GpuImage::_SetLayout(ArrayLevelIndex baseArrayLevel, USize32 arrayLevelCount, MipLevelIndex baseMipLevel, USize32 mipLevelCount, GpuImageLayout layout) {
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
