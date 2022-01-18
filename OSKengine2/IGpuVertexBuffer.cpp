#include "IGpuVertexBuffer.h"

using namespace OSK;

IGpuVertexBuffer::IGpuVertexBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: GpuDataBuffer(buffer, size, alignment) {

}
