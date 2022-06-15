#pragma once

#include "IGpuIndexBuffer.h"
#include "OglTypes.h"

namespace OSK::GRAPHICS {

	/// @deprecated OpenGL no implementado.
	class OSKAPI_CALL GpuIndexBufferOgl : public IGpuIndexBuffer {

	public:

		GpuIndexBufferOgl(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment, TSize numIndices);

		void _SetHandler(OglIndexBufferHandler handler);
		OglIndexBufferHandler GetHandler() const;

	private:

		OglIndexBufferHandler handler = OGL_NULL_HANDLER;

	};

}
