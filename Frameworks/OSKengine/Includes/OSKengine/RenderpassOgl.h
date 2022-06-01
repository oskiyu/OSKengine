#pragma once

#include "IRenderpass.h"
#include "OglTypes.h"

namespace OSK::GRAPHICS {

	/// @deprecated OpenGL no implementado.
	class OSKAPI_CALL RenderpassOgl : public IRenderpass {

	public:

		RenderpassOgl(RenderpassType type);
		~RenderpassOgl();

		void SetImages(GpuImage* image0, GpuImage* image1, GpuImage* image2) override;

		OglFramebufferHandler GetFramebuffer() const;
		OglRenderBufferHandler GetDepthStencilRenderBuffer() const;

	private:

		OglFramebufferHandler framebuffer = 0;

		//Para depth-stencil
		OglRenderBufferHandler renderbuffer[3] = { 0, 0, 0 };

	};

}
