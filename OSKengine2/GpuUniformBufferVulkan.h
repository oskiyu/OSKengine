#pragma once

#include "IGpuUniformBuffer.h"

namespace OSK {

	class OSKAPI_CALL GpuUniformBufferVulkan : public IGpuUniformBuffer {

	public:

		GpuUniformBufferVulkan(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);

	};

}
