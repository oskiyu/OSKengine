#include "GpuMemoryAllocatorOgl.h"

#include "GpuImageOgl.h"
#include "FormatOgl.h"
#include "GpuVertexBufferOgl.h"
#include "Vertex.h"
#include "GpuIndexBufferOgl.h"

#include <glad/glad.h>
#include "VertexOgl.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

GpuMemoryAllocatorOgl::GpuMemoryAllocatorOgl(IGpu* device)
	: IGpuMemoryAllocator(device) {

}

OwnedPtr<IGpuVertexBuffer> GpuMemoryAllocatorOgl::CreateVertexBuffer(const DynamicArray<Vertex3D>& vertices) {
	const TSize bufferSize = vertices.GetSize() * sizeof(Vertex3D);

	GpuVertexBufferOgl* output = new GpuVertexBufferOgl(nullptr, bufferSize, 0);

	OglVertexBufferHandler bufferHandler = 0; 
	OglVertexBufferHandler viewHandler = 0;

	glGenVertexArrays(1, &viewHandler);
	glGenBuffers(1, &bufferHandler);

	glBindVertexArray(viewHandler);
	glBindBuffer(GL_ARRAY_BUFFER, bufferHandler);

	glBufferData(GL_ARRAY_BUFFER, bufferSize, vertices.GetData(), GL_STATIC_DRAW);

	auto vertexAttribs = GetAttribInfoOgl_Vertex3D();
	for (TSize i = 0; i < vertexAttribs.GetSize(); i++) {
		VertexAttribInfo& info = vertexAttribs.At(i);

		glVertexAttribPointer(info.index, info.numberOfAttributes, info.type, GL_FALSE, info.stride, (void*)info.offset);
		glEnableVertexAttribArray(i);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	output->SetBufferHandler(bufferHandler);
	output->SetViewHandler(viewHandler);

	return output;
}

OwnedPtr<IGpuIndexBuffer> GpuMemoryAllocatorOgl::CreateIndexBuffer(const DynamicArray<TIndexSize>& indices) {
	const TSize bufferSize = sizeof(TIndexSize) * indices.GetSize();

	GpuIndexBufferOgl* output = new GpuIndexBufferOgl(nullptr, bufferSize, 0);

	OglIndexBufferHandler handler = OGL_NULL_HANDLER;
	glGenBuffers(1, &handler);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handler);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, indices.GetData(), GL_STATIC_DRAW);

	output->_SetHandler(handler);

	return output;
}

OwnedPtr<IGpuUniformBuffer> GpuMemoryAllocatorOgl::CreateUniformBuffer(TSize size) {
	OSK_ASSERT(false, "No implementado.");

	return nullptr;
}

OwnedPtr<GpuImage> GpuMemoryAllocatorOgl::CreateImage(const Vector3ui& size, GpuImageDimension dimension, TSize numLayers, Format format, GpuImageUsage usage, GpuSharedMemoryType sharedType, TSize msaaSamples, GpuImageSamplerDesc samplerDesc) {
	OSK_ASSERT(false, "No implementado.");
	return nullptr;
}

OwnedPtr<GpuDataBuffer> GpuMemoryAllocatorOgl::CreateStagingBuffer(TSize size) {
	OSK_ASSERT(false, "No implementado.");

	return nullptr;
}

IGpuMemoryBlock* GpuMemoryAllocatorOgl::GetNextBufferMemoryBlock(TSize size, GpuBufferUsage usage, GpuSharedMemoryType sharedType) {
	OSK_ASSERT(false, "No implementado.");

	return nullptr;
}
