#include "RenderpassOgl.h"

#include "RenderpassType.h"
#include "GpuImageOgl.h"
#include "GpuMemoryAllocatorOgl.h"
#include "OSKengine.h"
#include "RendererOgl.h"
#include "GpuMemoryTypes.h"
#include "Format.h"

#include <glad/glad.h>

using namespace OSK;

RenderpassOgl::RenderpassOgl(RenderpassType type)
	: IRenderpass(type) {
	if (type == RenderpassType::FINAL)
		framebuffer = OGL_DEFAULT_FRAMEBUFFER;
}

RenderpassOgl::~RenderpassOgl() {
	if (framebuffer)
		glDeleteFramebuffers(1, &framebuffer);
}

void RenderpassOgl::SetImage(GpuImage* image) {
	if (this->type == RenderpassType::FINAL)
		return;

	const auto size = image->GetSize();

	images[0] = image;
	images[1] = nullptr;

	colorImgs[0] = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(
		size.X, size.Y, image->GetFormat(),
		GpuImageUsage::COLOR, GpuSharedMemoryType::GPU_ONLY).GetPointer();

	depthImgs[0] = Engine::GetRenderer()->GetMemoryAllocator()->CreateImage(
		size.X, size.Y, Format::D32S8_SFLOAT_SUINT,
		GpuImageUsage::DEPTH_STENCIL, GpuSharedMemoryType::GPU_ONLY).GetPointer();

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, image->As<GpuImageOgl>()->GetHandler(), 0);

	glGenRenderbuffers(1, &renderbuffer[0]);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer[0]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.X, size.Y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer[0]);

	OSK_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
		"No se pudo crear el renderpass.");
}

void RenderpassOgl::SetImages(GpuImage* image0, GpuImage* image1, GpuImage* image2) {
	SetImage(image0);
}

OglFramebufferHandler RenderpassOgl::GetFramebuffer() const {
	return framebuffer;
}

OglRenderBufferHandler RenderpassOgl::GetDepthStencilRenderBuffer() const {
	return renderbuffer[0];
}
