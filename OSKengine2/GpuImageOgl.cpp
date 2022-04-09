#include "GpuImageOgl.h"

#include <glad/glad.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuImageOgl::GpuImageOgl(const Vector3ui& size, GpuImageDimension dimension, GpuImageUsage usage, TSize numLayers, Format format)
	: GpuImage(size, dimension, usage, numLayers, format) {

}

GpuImageOgl::~GpuImageOgl() {
	if (handler)
		glDeleteTextures(1, &handler);
}

OglImageHandler GpuImageOgl::GetHandler() const {
	return handler;
}

void GpuImageOgl::SetHandler(OglImageHandler handler) {
	this->handler = handler;
}
