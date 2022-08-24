#include "IGpuImage.h"

#include "IGpuMemoryBlock.h"
#include "IGpuMemorySubblock.h"
#include <glm.hpp>
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
}

GpuImage::~GpuImage() {
	if (buffer)
		block->RemoveSubblock(buffer);

	Engine::GetRenderer()->GetMemoryAllocator()->RemoveImageBlock(block.GetPointer());
	block.Delete();
}

void GpuImage::SetBlock(OwnedPtr<IGpuMemoryBlock> block) {
	this->block = block.GetPointer();
	buffer = block->GetNextMemorySubblock(block->GetAllocatedSize(), 0);
}

void GpuImage::SetData(const void* data, TSize size) {
	buffer->MapMemory();
	buffer->Write(data, size);
	buffer->Unmap();
}

void GpuImage::_SetPhysicalSize(const Vector3ui& size) {
	physicalSize = size;
}

Vector3ui GpuImage::GetSize() const {
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
	return block.GetPointer();
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

void GpuImage::SetLayout(GpuImageLayout layout) {
	currentLayout = layout;
}

GpuImageLayout GpuImage::GetLayout() const {
	return currentLayout;
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

IGpuImageView* GpuImage::GetView(SampledChannel channel, SampledArrayType arrayType, TSize baseArrayLevel, TSize layerCount, ViewUsage usage) const {
	auto it = views.Find(IGpuImageView(channel, arrayType, baseArrayLevel, layerCount, usage));

	if (!it.IsEmpty())
		return it.GetValue().second.GetPointer();

	OwnedPtr<IGpuImageView> view = CreateView(channel, arrayType, baseArrayLevel, layerCount, usage);
	views.Insert(IGpuImageView(channel, arrayType, baseArrayLevel, layerCount, usage), view.GetPointer());

	return view.GetPointer();
}
