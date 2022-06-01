#pragma once

#include <glad/glad.h>

namespace OSK::GRAPHICS {

	/// @deprecated OpenGL no implementado.
	using TOglHandler = GLuint;

	/// @deprecated OpenGL no implementado.
	using OglImageHandler = TOglHandler;
	/// @deprecated OpenGL no implementado.
	using OglFramebufferHandler = TOglHandler;
	/// @deprecated OpenGL no implementado.
	using OglRenderBufferHandler = TOglHandler;

	/// @deprecated OpenGL no implementado.
	using OglVertexBufferHandler = TOglHandler;
	/// @deprecated OpenGL no implementado.
	using OglIndexBufferHandler = TOglHandler;
	/// @deprecated OpenGL no implementado.
	using OglShaderHandler = TOglHandler;
	/// @deprecated OpenGL no implementado.
	using OglPipelineHandler = TOglHandler; // Shader program.

		/// @deprecated OpenGL no implementado.
	constexpr OglFramebufferHandler OGL_DEFAULT_FRAMEBUFFER = 0;
	/// @deprecated OpenGL no implementado.
	constexpr TOglHandler OGL_NULL_HANDLER = 0;

}
