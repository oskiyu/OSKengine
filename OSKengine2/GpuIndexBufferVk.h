#pragma once

#include "IGpuIndexBuffer.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuIndexBufferVk : public IGpuIndexBuffer {

	public:

		GpuIndexBufferVk(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment, TSize numIndices);

	};

}
