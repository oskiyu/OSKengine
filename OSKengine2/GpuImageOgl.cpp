#include "GpuImageOgl.h"

#include <glad/glad.h>

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuImageOgl::GpuImageOgl(unsigned int sizeX, unsigned int sizeY, Format format)
	: GpuImage(sizeX, sizeY, format) {

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
