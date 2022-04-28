#pragma once

namespace OSK::GRAPHICS {

	enum class RenderApiType {
		/// @deprecated OpenGL no implementado.
		OPENGL,
		VULKAN,
		DX12,
		OPENGL_ES
	};

}
