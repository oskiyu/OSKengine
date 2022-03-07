#include "IGpuIndexBuffer.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IGpuIndexBuffer::IGpuIndexBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: GpuDataBuffer(buffer, size, alignment) {

}
