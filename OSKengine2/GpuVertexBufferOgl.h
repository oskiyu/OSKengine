#pragma once

#include "IGpuVertexBuffer.h"

#include "OglTypes.h"

namespace OSK {

	class OSKAPI_CALL GpuVertexBufferOgl : public IGpuVertexBuffer {

	public:

		GpuVertexBufferOgl(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);

		void SetBufferHandler(OglVertexBufferHandler handler); 
		void SetViewHandler(OglVertexBufferHandler handler);

		OglVertexBufferHandler GetBufferHandler() const;
		OglVertexBufferHandler GetViewHandler() const;

	private:

		OglVertexBufferHandler bufferHandler = 0;
		OglVertexBufferHandler viewHandler = 0;

	};

}
