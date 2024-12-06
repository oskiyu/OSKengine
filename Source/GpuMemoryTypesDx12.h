#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "ApiCall.h"

enum D3D12_HEAP_TYPE;

namespace OSK::GRAPHICS {

	enum class GpuSharedMemoryType;

	D3D12_HEAP_TYPE OSKAPI_CALL GetGpuSharedMemoryTypeDx12(GpuSharedMemoryType type);

}

#endif
