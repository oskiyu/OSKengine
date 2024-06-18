#pragma once

#include "NumericTypes.h"

namespace OSK::GRAPHICS {

	struct GpuMemoryAlignments {
		USize32 minVertexBufferAlignment = 0;
		USize32 minIndexBufferAlignment = 0;

		USize32 minUniformBufferAlignment = 0;
		USize32 minStorageBufferAlignment = 0;
	};

}
