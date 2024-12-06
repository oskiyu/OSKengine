#include "GpuMemoryTypesDx12.h"

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "GpuMemoryTypes.h"

#include <d3d12.h>

D3D12_HEAP_TYPE OSK::GRAPHICS::GetGpuSharedMemoryTypeDx12(GpuSharedMemoryType type) {
	switch (type) {

	case GpuSharedMemoryType::GPU_AND_CPU:
		return D3D12_HEAP_TYPE_UPLOAD;

	case GpuSharedMemoryType::GPU_ONLY:
		return D3D12_HEAP_TYPE_DEFAULT;

	}

	return (D3D12_HEAP_TYPE)0;
}

#endif
