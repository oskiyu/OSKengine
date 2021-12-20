#pragma once

#include <glad/glad.h>

namespace OSK {

	using TOglHandler = GLuint;

	using OglImageHandler = TOglHandler;
	using OglFramebufferHandler = TOglHandler;
	using OglRenderBufferHandler = TOglHandler;

	constexpr OglFramebufferHandler oglDefaultFramebuffer = 0;
	constexpr TOglHandler oglNullHandler = 0;

}
