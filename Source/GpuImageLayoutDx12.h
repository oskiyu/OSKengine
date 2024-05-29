#pragma once

#include "ApiCall.h"

enum D3D12_RESOURCE_STATES;

namespace OSK::GRAPHICS {

	enum class GpuImageLayout;

	D3D12_RESOURCE_STATES OSKAPI_CALL GetGpuImageLayoutDx12(GpuImageLayout layout);

}
