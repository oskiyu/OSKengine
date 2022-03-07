#include "IGpuVertexBuffer.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IGpuVertexBuffer::IGpuVertexBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: GpuDataBuffer(buffer, size, alignment) {

}
