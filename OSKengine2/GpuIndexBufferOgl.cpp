#include "GpuIndexBufferOgl.h"

using namespace OSK;

GpuIndexBufferOgl::GpuIndexBufferOgl(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment)
	: IGpuIndexBuffer(buffer, size, alignment) {

}

void GpuIndexBufferOgl::_SetHandler(OglIndexBufferHandler handler) {
	this->handler = handler;
}

OglIndexBufferHandler GpuIndexBufferOgl::GetHandler() const {
	return handler;
}
