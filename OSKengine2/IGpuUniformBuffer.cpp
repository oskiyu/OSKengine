#include "IGpuUniformBuffer.h"

using namespace OSK;

IGpuUniformBuffer::IGpuUniformBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: GpuDataBuffer(buffer, size, alignment) {

}
