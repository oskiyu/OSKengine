#pragma once

#include <glad/glad.h>

namespace OSK {

	using TOglHandler = GLuint;

	using OglImageHandler = TOglHandler;
	using OglFramebufferHandler = TOglHandler;
	using OglRenderBufferHandler = TOglHandler;

	using OglVertexBufferHandler = TOglHandler;
	using OglIndexBufferHandler = TOglHandler;
	using OglShaderHandler = TOglHandler;
	using OglPipelineHandler = TOglHandler; // Shader program.
	
	constexpr OglFramebufferHandler OGL_DEFAULT_FRAMEBUFFER = 0;
	constexpr TOglHandler OGL_NULL_HANDLER = 0;

}
