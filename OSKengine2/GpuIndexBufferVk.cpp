#include "GpuIndexBufferVk.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuIndexBufferVk::GpuIndexBufferVk(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment, TSize numIndices)
	: IGpuIndexBuffer(buffer, size, alignment, numIndices) {

}
