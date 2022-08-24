#include "IGpuStorageBuffer.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IGpuStorageBuffer::IGpuStorageBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: GpuDataBuffer(buffer, size, alignment) {

}
