#pragma once

#include "IGpuVertexBuffer.h"

namespace OSK {

	class OSKAPI_CALL GpuVertexBufferVulkan : public IGpuVertexBuffer {

	public:

		GpuVertexBufferVulkan(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);

	};

}
