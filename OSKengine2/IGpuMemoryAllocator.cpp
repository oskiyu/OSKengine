#include "IGpuMemoryAllocator.h"

#include "IGpuMemoryBlock.h"
#include "IGpuMemorySubblock.h"
#include "IGpuDataBuffer.h"
#include "IGpuImage.h"
#include "GpuImageUsage.h"
#include "GpuImageDimensions.h"
#include "Format.h"

#include "OSKengine.h"
#include "Logger.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

TSize IGpuMemoryAllocator::SizeOfMemoryBlockInMb = 16;

bool GpuBufferMemoryBlockInfo::operator==(const GpuBufferMemoryBlockInfo& other) const {
	return size == other.size && usage == other.usage && sharedType == other.sharedType;
}

IGpuMemoryAllocator::IGpuMemoryAllocator(IGpu* device) : device(device) {
	bufferMemoryBlocks = {};
}

IGpuMemoryAllocator::~IGpuMemoryAllocator() {
	for (auto& list : bufferMemoryBlocks)
		for (auto& block : list.second)
			delete block.GetPointer();

	for (auto& i : imageMemoryBlocks)
		delete i.GetPointer();
}

void IGpuMemoryAllocator::RemoveImageBlock(IGpuMemoryBlock* iblock) {
	imageMemoryBlocks.Remove(OwnedPtr(iblock));
	}

void IGpuMemoryAllocator::Free() {
	for (auto& list : bufferMemoryBlocks) {
		for (auto& i : list.second)
			i->Free();

		list.second.Free();
	}

	for (auto i : imageMemoryBlocks)
		i->Free();

	imageMemoryBlocks.Free();
}
OwnedPtr<GpuImage> IGpuMemoryAllocator::CreateCubemapImage(const Vector2ui& faceSize, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, GpuImageSamplerDesc samplerDesc) {
	if (!EFTraits::HasFlag(usage, GpuImageUsage::CUBEMAP))
		EFTraits::AddFlag(&usage, GpuImageUsage::CUBEMAP);

	GpuImageCreateInfo info = GpuImageCreateInfo::CreateDefault2D(faceSize, format, usage);
	info.memoryType = sharedType;
	info.samplerDesc = samplerDesc;
	info.numLayers = 6;

	return CreateImage(info);
}

IGpuMemoryBlock* IGpuMemoryAllocator::GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	// Miramos todas las listas de bloques para comprobar si hay alguna que
	// tiene bloques con las características necesarias.
	for (auto& [blockInfo, list] : bufferMemoryBlocks) {
		if (blockInfo.usage == usage && blockInfo.sharedType == sharedType) {
			for (auto& i : list)
				if (i->GetAvailableSpace() >= size)
					return i.GetPointer();

			break;
		}
	}

	// Creamos uno nuevo
	if (size < IGpuMemoryAllocator::SizeOfMemoryBlockInMb * 1024 * 1024)
		size = IGpuMemoryAllocator::SizeOfMemoryBlockInMb * 1024 * 1024;

	// Creamos un nuevo bloque.
	OwnedPtr<IGpuMemoryBlock> newBlock = CreateNewBufferBlock(size, usage, sharedType);

	// Engine::GetLogger()->InfoLog("Creado bloque de memoria GPU.");
	// Engine::GetLogger()->InfoLog("	Tipo: " + ToString(sharedType)); @bug ???
	// Engine::GetLogger()->InfoLog("	Uso: " + ToString(usage)); @bug ???
	// Engine::GetLogger()->InfoLog("	Tamaño: " + std::to_string(size));

	LinkedList<OwnedPtr<IGpuMemoryBlock>> list;
	list.Insert(OwnedPtr<IGpuMemoryBlock>(newBlock.GetPointer()));

	bufferMemoryBlocks.Insert(Pair{
		GpuBufferMemoryBlockInfo{size, usage, sharedType },
		list
		});

	return newBlock.GetPointer();
}

IGpuMemorySubblock* IGpuMemoryAllocator::GetNextBufferMemorySubblock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	return GetNextBufferMemoryBlock(size, usage, sharedType)->GetNextMemorySubblock(size, 0);
}

GpuImageCreateInfo GpuImageCreateInfo::CreateDefault1D(uint32_t resolution, Format format, GpuImageUsage usage) {
	GpuImageCreateInfo output{};
	output.resolution = Vector3ui{ resolution, 1, 1 };
	output.format = format;
	output.usage = usage;
	output.dimension = GpuImageDimension::d1D;

	return output;
}

GpuImageCreateInfo GpuImageCreateInfo::CreateDefault2D(const Vector2ui& resolution, Format format, GpuImageUsage usage) {
	GpuImageCreateInfo output{};
	output.resolution = Vector3ui{ resolution.X, resolution.Y, 1 };
	output.format = format;
	output.usage = usage;
	output.dimension = GpuImageDimension::d2D;

	return output;
}

GpuImageCreateInfo GpuImageCreateInfo::CreateDefault3D(const Vector3ui& resolution, Format format, GpuImageUsage usage) {
	GpuImageCreateInfo output{};
	output.resolution = resolution;
	output.format = format;
	output.usage = usage;
	output.dimension = GpuImageDimension::d3D;

	return output;
}

void GpuImageCreateInfo::SetMsaaSamples(TSize msaaSamples) {
	this->msaaSamples = msaaSamples;
}

void GpuImageCreateInfo::SetSamplerDescription(const GpuImageSamplerDesc& description) {
	samplerDesc = description;
}

void GpuImageCreateInfo::SetMemoryType(GpuSharedMemoryType memoryType) {
	this->memoryType = memoryType;
}
