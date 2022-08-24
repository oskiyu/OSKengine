#include "GpuStorageBufferVk.h"

OSK::GRAPHICS::GpuStorageBufferVk::GpuStorageBufferVk(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: IGpuStorageBuffer(buffer, size, alignment) {

}
