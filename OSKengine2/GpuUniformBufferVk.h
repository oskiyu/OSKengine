#pragma once

#include "IGpuUniformBuffer.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuUniformBufferVk : public IGpuUniformBuffer {

	public:

		GpuUniformBufferVk(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);

	};

}
