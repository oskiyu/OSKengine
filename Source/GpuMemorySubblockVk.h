#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "IGpuMemorySubblock.h"


namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuMemorySubblockVk : public IGpuMemorySubblock {

	public:

		GpuMemorySubblockVk(IGpuMemoryBlock* owner, UIndex64 size, UIndex64 offset);

	};

}

#endif
