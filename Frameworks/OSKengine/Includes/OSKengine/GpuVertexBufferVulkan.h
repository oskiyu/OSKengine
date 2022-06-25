#pragma once

#include "IGpuVertexBuffer.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuVertexBufferVulkan : public IGpuVertexBuffer {

	public:

		GpuVertexBufferVulkan(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment, TSize numVertices, const VertexInfo& vertexInfo);

	};

}
