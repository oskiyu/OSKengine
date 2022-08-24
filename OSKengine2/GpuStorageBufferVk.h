#pragma once

#include "IGpuStorageBuffer.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuStorageBufferVk : public IGpuStorageBuffer {

	public:

		GpuStorageBufferVk(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);

	};

}
