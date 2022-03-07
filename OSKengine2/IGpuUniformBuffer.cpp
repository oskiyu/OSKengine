#include "IGpuUniformBuffer.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IGpuUniformBuffer::IGpuUniformBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: GpuDataBuffer(buffer, size, alignment) {

}
