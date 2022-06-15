#include "GpuIndexBufferOgl.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuIndexBufferOgl::GpuIndexBufferOgl(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment, TSize numIndices)
	: IGpuIndexBuffer(buffer, size, alignment, numIndices) {

}

void GpuIndexBufferOgl::_SetHandler(OglIndexBufferHandler handler) {
	this->handler = handler;
}

OglIndexBufferHandler GpuIndexBufferOgl::GetHandler() const {
	return handler;
}
