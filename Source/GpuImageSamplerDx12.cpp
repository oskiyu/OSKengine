#include "GpuImageSamplerDx12.h"

#ifdef OSK_USE_DIRECTX12_BACKEND

#include "Assert.h"
#include "UnreachableException.h"

#include "GpuDx12.h"

D3D12_FILTER GetFilterDx12(OSK::GRAPHICS::GpuImageFilteringType type) {
	switch (type) {
	case OSK::GRAPHICS::GpuImageFilteringType::LINEAR:
		return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	case OSK::GRAPHICS::GpuImageFilteringType::NEAREST:
		return D3D12_FILTER_MIN_MAG_MIP_POINT;
	case OSK::GRAPHICS::GpuImageFilteringType::CUBIC:
		return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	}

	OSK_ASSERT(false, OSK::UnreachableException("GpuImageFilteringType no reconocido."));
}

D3D12_TEXTURE_ADDRESS_MODE GetAddressModeDx12(OSK::GRAPHICS::GpuImageAddressMode mode) {
	switch (mode) {
	case OSK::GRAPHICS::GpuImageAddressMode::REPEAT:
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	case OSK::GRAPHICS::GpuImageAddressMode::MIRRORED_REPEAT:
		return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
	case OSK::GRAPHICS::GpuImageAddressMode::EDGE:
		return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	case OSK::GRAPHICS::GpuImageAddressMode::BACKGROUND_BLACK:
		return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	case OSK::GRAPHICS::GpuImageAddressMode::BACKGROUND_WHITE:
		return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	}

	OSK_ASSERT(false, OSK::UnreachableException("GpuImageAddressMode no reconocido."));
}

OSK::GRAPHICS::GpuImageSamplerDx12::GpuImageSamplerDx12(const GpuImageSamplerDesc& info, const GpuDx12* gpu) : IGpuImageSampler(info) {
	D3D12_SAMPLER_DESC desc{};
	desc.MinLOD = static_cast<float>(info.minMipLevel);
	desc.MaxLOD = static_cast<float>(info.maxMipLevel);
	desc.Filter = GetFilterDx12(info.filteringType);
	desc.AddressU = GetAddressModeDx12(info.addressMode);
	desc.AddressV = GetAddressModeDx12(info.addressMode);
	desc.AddressW = GetAddressModeDx12(info.addressMode);
	desc.MaxAnisotropy = 16.0f;
	desc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	desc.MipLODBias = -0.5f;

	if (info.addressMode == OSK::GRAPHICS::GpuImageAddressMode::BACKGROUND_BLACK) {
		desc.BorderColor[0] = 0.0f;
		desc.BorderColor[1] = 0.0f;
		desc.BorderColor[2] = 0.0f;
		desc.BorderColor[3] = 1.0f;
	}
	else {
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle = {}; // AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	gpu->GetDevice()->CreateSampler(&desc, handle);
}

OSK::GRAPHICS::GpuImageSamplerDx12::~GpuImageSamplerDx12() {

}

#endif
