#include "GpuIndexBufferVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuIndexBufferVulkan::GpuIndexBufferVulkan(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment, TSize numIndices)
	: IGpuIndexBuffer(buffer, size, alignment, numIndices) {

}
