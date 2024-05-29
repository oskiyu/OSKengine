#pragma once

#include "ApiCall.h"

enum D3D12_HEAP_TYPE;

namespace OSK::GRAPHICS {

	enum class GpuSharedMemoryType;

	D3D12_HEAP_TYPE OSKAPI_CALL GetGpuSharedMemoryTypeDx12(GpuSharedMemoryType type);

}
