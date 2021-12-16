#include "GpuImageLayoutDx12.h"

#include "GpuImageLayout.h"
#include <d3d12.h>

D3D12_RESOURCE_STATES OSK::GetGpuImageLayoutDx12(OSK::GpuImageLayout layout) {
	switch (layout) {
		case OSK::GpuImageLayout::UNDEFINED:
			return D3D12_RESOURCE_STATE_COMMON;

		case OSK::GpuImageLayout::PRESENT:
			return D3D12_RESOURCE_STATE_PRESENT;

		case OSK::GpuImageLayout::COLOR_ATTACHMENT:
			return D3D12_RESOURCE_STATE_RENDER_TARGET;

		case OSK::GpuImageLayout::DEPTH_STENCIL_TARGET:
			return D3D12_RESOURCE_STATE_DEPTH_WRITE;

		case OSK::GpuImageLayout::DEPTH_STENCIL_READ_ONLY:
			return D3D12_RESOURCE_STATE_DEPTH_READ;

		case OSK::GpuImageLayout::SHADER_READ_ONLY:
			return D3D12_RESOURCE_STATE_GENERIC_READ;

		case OSK::GpuImageLayout::TRANSFER_SOURCE:
			return D3D12_RESOURCE_STATE_COPY_SOURCE;

		case OSK::GpuImageLayout::TRANSFER_DESTINATION:
			return D3D12_RESOURCE_STATE_COPY_DEST;

		default:
			return D3D12_RESOURCE_STATE_COMMON;
	}
}
