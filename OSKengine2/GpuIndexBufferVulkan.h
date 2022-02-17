#pragma once

#include "IGpuIndexBuffer.h"

namespace OSK {

	class OSKAPI_CALL GpuIndexBufferVulkan : public IGpuIndexBuffer {

	public:

		GpuIndexBufferVulkan(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);

	};

}
