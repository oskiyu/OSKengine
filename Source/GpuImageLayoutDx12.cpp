#include "GpuImageLayoutDx12.h"

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "GpuImageLayout.h"
#include <d3d12.h>

D3D12_RESOURCE_STATES OSK::GRAPHICS::GetGpuImageLayoutDx12(GpuImageLayout layout) {
	switch (layout) {
		case GpuImageLayout::UNDEFINED:
			return D3D12_RESOURCE_STATE_GENERIC_READ;

		case GpuImageLayout::PRESENT:
			return D3D12_RESOURCE_STATE_PRESENT;

		case GpuImageLayout::COLOR_ATTACHMENT:
			return D3D12_RESOURCE_STATE_RENDER_TARGET;

		case GpuImageLayout::DEPTH_STENCIL_TARGET:
			return D3D12_RESOURCE_STATE_DEPTH_WRITE;

		case GpuImageLayout::DEPTH_STENCIL_READ_ONLY:
			return D3D12_RESOURCE_STATE_DEPTH_READ;

		case GpuImageLayout::SAMPLED:
			return D3D12_RESOURCE_STATE_GENERIC_READ;

		case GpuImageLayout::TRANSFER_SOURCE:
			return D3D12_RESOURCE_STATE_COPY_SOURCE;

		case GpuImageLayout::TRANSFER_DESTINATION:
			return D3D12_RESOURCE_STATE_COPY_DEST;

		case GpuImageLayout::GENERAL:
			return D3D12_RESOURCE_STATE_COMMON;

		default:
			return D3D12_RESOURCE_STATE_COMMON;
	}
}

#endif
