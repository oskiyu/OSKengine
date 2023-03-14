#pragma once

#include "IGpuVertexBuffer.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuVertexBufferVk : public IGpuVertexBuffer {

	public:

		GpuVertexBufferVk(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment, TSize numVertices, const VertexInfo& vertexInfo);

	};

}
