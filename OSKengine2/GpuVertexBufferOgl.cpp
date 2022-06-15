#include "GpuVertexBufferOgl.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuVertexBufferOgl::GpuVertexBufferOgl(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment, TSize numVertices, const VertexInfo& vertexInfo)
	: IGpuVertexBuffer(buffer, size, alignment, numVertices, vertexInfo) {

}

void GpuVertexBufferOgl::SetBufferHandler(OglVertexBufferHandler handler) {
	bufferHandler = handler;
}

void GpuVertexBufferOgl::SetViewHandler(OglVertexBufferHandler handler) {
	viewHandler = handler;
}

OglVertexBufferHandler GpuVertexBufferOgl::GetBufferHandler() const {
	return bufferHandler;
}

OglVertexBufferHandler GpuVertexBufferOgl::GetViewHandler() const {
	return viewHandler;
}
