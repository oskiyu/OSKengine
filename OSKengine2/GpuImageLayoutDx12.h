#pragma once

#include "OSKmacros.h"

enum D3D12_RESOURCE_STATES;

namespace OSK {

	enum class GpuImageLayout;

	D3D12_RESOURCE_STATES OSKAPI_CALL GetGpuImageLayoutDx12(GpuImageLayout layout);

}
