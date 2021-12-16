#include "IGpuImage.h"

#include "IGpuMemoryBlock.h"
#include "IGpuMemorySubblock.h"
#include <glm.hpp>
#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;

GpuImage::GpuImage(unsigned int sizeX, unsigned int sizeY, Format format)
	: format(format) {
	size = { sizeX, sizeY };
}

GpuImage::~GpuImage() {
	if (buffer)
		block->RemoveSubblock(buffer);
}

void GpuImage::SetBlock(OwnedPtr<IGpuMemoryBlock> block) {
	this->block = block.GetPointer();
	buffer = block->GetNextMemorySubblock(block->GetAllocatedSize());
}

void GpuImage::SetData(const void* data, TSize size) {
	buffer->MapMemory();
	buffer->Write(data, size);
	buffer->Unmap();
}

Vector2ui GpuImage::GetSize() const {
	return size;
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
	return 1;
	return glm::floor(glm::log2(glm::max((float)sizeX, (float)sizeY))) + 1;
}
