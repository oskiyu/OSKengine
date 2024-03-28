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

GpuImage::GpuImage(const Vector3ui& size, GpuImageDimension dimension, GpuImageUsage usage, USize32 numLayers, Format format, USize32 numSamples, GpuImageSamplerDesc samplerDesc, GpuImageTiling tiling)
	: m_size(size), m_samplerDesc(samplerDesc), m_numSamples(numSamples), m_format(format), m_currentLayout(GpuImageLayout::UNDEFINED), m_dimension(dimension), m_usage(usage), m_numLayers(numLayers), m_tiling(tiling) {
	
	switch (samplerDesc.mipMapMode) {
	case GpuImageMipmapMode::AUTO:
		m_mipLevels = GetMipLevels(size.x, size.y);
		break;
	case GpuImageMipmapMode::CUSTOM:
		m_mipLevels = samplerDesc.maxMipLevel;
		break;
	case GpuImageMipmapMode::NONE:
		m_mipLevels = 1;
		break;
	}

	_SetPhysicalSize(size);
	_InitDefaultLayout();
}

GpuImage::~GpuImage() {
	if (m_buffer)
		m_block->RemoveSubblock(m_buffer);

	Engine::GetRenderer()->GetAllocator()->RemoveMemoryBlock(m_block);
}

void GpuImage::SetBlock(OwnedPtr<IGpuMemoryBlock> block) {
	m_block = block.GetPointer();
	m_buffer = block->GetNextMemorySubblock(block->GetAllocatedSize(), 0);
}

void GpuImage::_SetPhysicalSize(const Vector3ui& size) {
	m_physicalSize = size;
}

Vector3ui GpuImage::GetSize3D() const {
	return m_size;
}

Vector2ui GpuImage::GetSize2D() const {
	return { m_size.x, m_size.y };
}

USize32 GpuImage::GetSize1D() const {
	return m_size.x;
}

Vector2ui GpuImage::GetMipLevelSize2D(UIndex32 mipLevel) const {
	Vector2ui size = GetSize2D();

	for (UIndex32 i = 0; i < mipLevel; i++)
		size /= 2;

	return size;
}

Vector3ui GpuImage::GetPhysicalSize() const {
	return m_physicalSize;
}

GpuImageDimension GpuImage::GetDimension() const {
	return m_dimension;
}

GpuImageUsage GpuImage::GetUsage() const {
	return m_usage;
}

USize32 GpuImage::GetNumLayers() const {
	return m_numLayers;
}

GpuImageSamplerDesc GpuImage::GetImageSampler() const {
	return m_samplerDesc;
}

Format GpuImage::GetFormat() const {
	return m_format;
}

IGpuMemoryBlock* GpuImage::GetMemory() const {
	return m_block;
}

IGpuMemorySubblock* GpuImage::GetBuffer() const {
	return m_buffer;
}

unsigned int GpuImage::GetMipLevels() const {
	return m_mipLevels;
}

USize32 GpuImage::GetMipLevels(uint32_t sizeX, uint32_t sizeY) {
	return static_cast<USize32>(glm::floor(glm::log2(glm::max((float)sizeX, (float)sizeY)))) + 1;
}

USize64 GpuImage::GetNumberOfBytes() const {
	return static_cast<USize64>(m_size.x) * static_cast<USize64>(m_size.y) * static_cast<USize64>(m_size.z) * GetFormatNumberOfBytes(GetFormat());
}

USize64 GpuImage::GetPhysicalNumberOfBytes() const {
	return static_cast<USize64>(m_physicalSize.x) * m_physicalSize.y * m_physicalSize.z * GetFormatNumberOfBytes(GetFormat());
}

USize32 GpuImage::GetNumSamples() const {
	return m_numSamples;
}

const IGpuImageView* GpuImage::GetView(const GpuImageViewConfig& viewConfig) const {
	if (m_views.contains(viewConfig))
		return m_views.at(viewConfig).GetPointer();

	OwnedPtr<IGpuImageView> view = CreateView(viewConfig);
	m_views[viewConfig] = view.GetPointer();

	return view.GetPointer();
}
void GpuImage::SetCurrentBarrier(const GpuBarrierInfo& barrier) {
	m_currentBarrier = barrier;
}

const GpuBarrierInfo& GpuImage::GetCurrentBarrier() const {
	return m_currentBarrier;
}

void GpuImage::_InitDefaultLayout() {
	const USize32 arrayLevelCount = m_numLayers;
	const USize32 mipLevelCount = m_mipLevels;

	for (ArrayLevelIndex a = 0; a < arrayLevelCount; a++) {
		std::unordered_map<GpuImage::MipLevelIndex, GpuImageLayout> arrayLevel;
		for (MipLevelIndex m = 0; m < mipLevelCount; m++)
			arrayLevel[m] = GpuImageLayout::UNDEFINED;

		m_layouts[a] = arrayLevel;
	}
}

void GpuImage::_SetLayout(ArrayLevelIndex baseArrayLevel, USize32 arrayLevelCount, MipLevelIndex baseMipLevel, USize32 mipLevelCount, GpuImageLayout layout) {
	arrayLevelCount = glm::min(arrayLevelCount, m_numLayers);
	mipLevelCount = glm::min(mipLevelCount, m_mipLevels);
	
	for (ArrayLevelIndex a = baseArrayLevel; a < baseArrayLevel + arrayLevelCount; a++) {
		auto& arrayLevel = m_layouts.at(a);

		for (MipLevelIndex m = baseMipLevel; m < baseMipLevel + mipLevelCount; m++)
			arrayLevel[m] = layout;
	}
}
GpuImageLayout GpuImage::_GetLayout(ArrayLevelIndex arrayLevel, MipLevelIndex mipLevel) const {
	return m_layouts.at(arrayLevel).at(mipLevel);
}
