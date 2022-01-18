#include "GpuMemoryAllocatorOgl.h"

#include "GpuImageOgl.h"
#include "FormatOgl.h"
#include "GpuVertexBufferOgl.h"
#include "Vertex.h"

#include <glad/glad.h>

using namespace OSK;

GpuMemoryAllocatorOgl::GpuMemoryAllocatorOgl(IGpu* device)
	: IGpuMemoryAllocator(device) {

}

OwnedPtr<IGpuVertexBuffer> GpuMemoryAllocatorOgl::CreateVertexBuffer(const DynamicArray<Vertex3D>& vertices) {
	const TSize bufferSize = vertices.GetSize() * sizeof(Vertex3D);

	GpuVertexBufferOgl* output = new GpuVertexBufferOgl(nullptr, bufferSize, 0);

	OglVertexBufferHandler bufferHandler = 0; 
	OglVertexBufferHandler viewHandler = 0;

	glGenBuffers(1, &bufferHandler);
	glGenVertexArrays(1, &viewHandler);

	glBindVertexArray(viewHandler);
	glBindBuffer(GL_ARRAY_BUFFER, bufferHandler);

	glBufferData(GL_ARRAY_BUFFER, bufferSize, vertices.GetData(), GL_STATIC_DRAW);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	output->SetBufferHandler(bufferHandler);
	output->SetViewHandler(viewHandler);

	return output;
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

OwnedPtr<GpuDataBuffer> GpuMemoryAllocatorOgl::CreateStagingBuffer(TSize size) {
	OSK_ASSERT(false, "No implementado.");

	return nullptr;
}

IGpuMemoryBlock* GpuMemoryAllocatorOgl::GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	OSK_ASSERT(false, "No implementado.");

	return nullptr;
}
