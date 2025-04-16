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

GpuImage::GpuImage(const GpuImageCreateInfo& info, const ICommandQueue* ownerQueue) : m_imageInfo(info), m_ownerQueue(ownerQueue) {
	
	switch (info.samplerDesc.mipMapMode) {
	case GpuImageMipmapMode::AUTO:
		m_mipLevels = GetMipLevels(info.resolution.x, info.resolution.y);
		break;
	case GpuImageMipmapMode::CUSTOM:
		m_mipLevels = info.samplerDesc.maxMipLevel;
		break;
	case GpuImageMipmapMode::NONE:
		m_mipLevels = 1;
		break;
	}

	_SetPhysicalSize(info.resolution);
	_InitDefaultLayout();
}

GpuImage::~GpuImage() {
	Engine::GetRenderer()->GetAllocator()->RemoveMemoryBlock(m_block.GetPointer());
}

void GpuImage::SetBlock(UniquePtr<IGpuMemoryBlock>&& block) {
	m_buffer = block->GetNextMemorySubblock(block->GetAllocatedSize(), 0);
	m_block = std::move(block);
}

void GpuImage::_SetPhysicalSize(const Vector3ui& size) {
	m_physicalSize = size;
}

Vector3ui GpuImage::GetSize3D() const {
	return m_imageInfo.resolution;
}

Vector2ui GpuImage::GetSize2D() const {
	return { m_imageInfo.resolution.x, m_imageInfo.resolution.y };
}

USize32 GpuImage::GetSize1D() const {
	return m_imageInfo.resolution.x;
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
	return m_imageInfo.dimension;
}

GpuImageUsage GpuImage::GetUsage() const {
	return m_imageInfo.usage;
}

USize32 GpuImage::GetNumLayers() const {
	return m_imageInfo.numLayers;
}

GpuImageSamplerDesc GpuImage::GetImageSampler() const {
	return m_imageInfo.samplerDesc;
}

Format GpuImage::GetFormat() const {
	return m_imageInfo.format;
}

const IGpuMemoryBlock* GpuImage::GetMemory() const {
	return m_block.GetPointer();
}

IGpuMemoryBlock* GpuImage::GetMemory() {
	return m_block.GetPointer();
}

const IGpuMemorySubblock* GpuImage::GetBuffer() const {
	return m_buffer.GetPointer();
}

IGpuMemorySubblock* GpuImage::GetBuffer() {
	return m_buffer.GetPointer();
}

unsigned int GpuImage::GetMipLevels() const {
	return m_mipLevels;
}

USize32 GpuImage::GetMipLevels(uint32_t sizeX, uint32_t sizeY) {
	return static_cast<USize32>(glm::floor(glm::log2(glm::max((float)sizeX, (float)sizeY)))) + 1;
}

USize64 GpuImage::GetNumberOfBytes() const {
	return 
		static_cast<USize64>(m_imageInfo.resolution.x) * 
		static_cast<USize64>(m_imageInfo.resolution.y) * 
		static_cast<USize64>(m_imageInfo.resolution.z) * GetFormatNumberOfBytes(GetFormat());
}

USize64 GpuImage::GetPhysicalNumberOfBytes() const {
	return 
		static_cast<USize64>(m_physicalSize.x) * 
		m_physicalSize.y * 
		m_physicalSize.z * 
		GetFormatNumberOfBytes(GetFormat());
}

USize32 GpuImage::GetMsaaSampleCount() const {
	return m_imageInfo.msaaSamples;
}

GpuImageTiling GpuImage::GetImageTiling() const {
	return m_imageInfo.tilingType;
}

const IGpuImageView* GpuImage::GetView(const GpuImageViewConfig& viewConfig) const {
	if (m_views.contains(viewConfig))
		return m_views.at(viewConfig).GetPointer();

	UniquePtr<IGpuImageView> view = CreateView(viewConfig);
	const auto* output = view.GetPointer();

	m_views[viewConfig] = std::move(view);

	return output;
}
void GpuImage::SetCurrentBarrier(const GpuBarrierInfo& barrier) {
	m_currentBarrier = barrier;
}

const GpuBarrierInfo& GpuImage::GetCurrentBarrier() const {
	return m_currentBarrier;
}

void GpuImage::_InitDefaultLayout() {
	const USize32 arrayLevelCount = m_imageInfo.numLayers;
	const USize32 mipLevelCount = m_mipLevels;

	for (ArrayLevelIndex a = 0; a < arrayLevelCount; a++) {
		std::unordered_map<GpuImage::MipLevelIndex, GpuImageLayout> arrayLevel;
		for (MipLevelIndex m = 0; m < mipLevelCount; m++)
			arrayLevel[m] = GpuImageLayout::UNDEFINED;

		m_layouts[a] = arrayLevel;
	}
}

void GpuImage::_SetLayout(ArrayLevelIndex baseArrayLevel, USize32 arrayLevelCount, MipLevelIndex baseMipLevel, USize32 mipLevelCount, GpuImageLayout layout) {
	arrayLevelCount = glm::min(arrayLevelCount, m_imageInfo.numLayers);
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

const ICommandQueue* GpuImage::GetOwnerQueue() const {
	return m_ownerQueue;
}

void GpuImage::_UpdateOwnerQueue(const ICommandQueue* ownerQueue) {
	m_ownerQueue = ownerQueue;
}
