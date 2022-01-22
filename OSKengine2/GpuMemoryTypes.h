#pragma once

#include "EnumFlags.hpp"

namespace OSK {

	enum class GpuSharedMemoryType {
		GPU_AND_CPU,
		GPU_ONLY
	};

	enum class GpuMemoryUsage {
		BUFFER,
		IMAGE
	};

	enum class GpuBufferUsage {
		UNIFORM_BUFFER = 1,
		VERTEX_BUFFER = 2,
		INDEX_BUFFER = 4,
		TRANSFER_SOURCE = 8,
		TRANSFER_DESTINATION = 16
	};

	enum class GpuImageUsage {
		COLOR = 1,
		DEPTH_STENCIL = 2,
		SAMPLED = 4,
		TRANSFER_SOURCE = 8,
		TRANSFER_DESTINATION = 16
	};

}

OSK_FLAGS(OSK::GpuBufferUsage);
OSK_FLAGS(OSK::GpuImageUsage);
