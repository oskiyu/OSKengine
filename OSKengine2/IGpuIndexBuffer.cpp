#include "IGpuIndexBuffer.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

IGpuIndexBuffer::IGpuIndexBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment, TSize numIndices)
	: GpuDataBuffer(buffer, size, alignment), numIndices(numIndices) {

}

TSize IGpuIndexBuffer::GetNumIndices() const {
	return numIndices;
}

TSize IGpuIndexBuffer::GetNumTriangles() const {
	return numIndices / 3;
}
