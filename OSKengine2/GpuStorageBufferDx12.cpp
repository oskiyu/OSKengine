#include "GpuStorageBufferDx12.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuStorageBufferDx12::GpuStorageBufferDx12(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: IGpuStorageBuffer(buffer, size, alignment) {

}
