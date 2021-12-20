#include "GpuMemoryAllocatorOgl.h"

#include "GpuImageOgl.h"
#include "FormatOgl.h"

#include <glad/glad.h>

using namespace OSK;

GpuMemoryAllocatorOgl::GpuMemoryAllocatorOgl(IGpu* device)
	: IGpuMemoryAllocator(device) {

}

OwnedPtr<GpuImage> GpuMemoryAllocatorOgl::CreateImage(unsigned int sizeX, unsigned int sizeY, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType) {
	GpuImageOgl* image = new GpuImageOgl(sizeX, sizeY, format);

	OglImageHandler handler = 0;
	glGenTextures(1, &handler);
	glBindTexture(GL_TEXTURE_2D, handler);

	glTexImage2D(GL_TEXTURE_2D, 0, GetFormatOgl(format), sizeX, sizeY, 0, GetFormatOgl(format), GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	image->SetHandler(handler);

	return image;
}
