#pragma once

#include "IGpuStorageBuffer.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuStorageBufferDx12 : public IGpuStorageBuffer {

	public:

		GpuStorageBufferDx12(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);
		
	};

}
