#include "IGpuIndexBuffer.h"

using namespace OSK;

IGpuIndexBuffer::IGpuIndexBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: GpuDataBuffer(buffer, size, alignment) {

}
