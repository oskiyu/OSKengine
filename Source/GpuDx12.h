#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>
#include <dxgi1_6.h>

#include "ApiCall.h"
#include "IGpu.h"
#include "UniquePtr.hpp"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuDx12 final : public IGpu {

	public:

		~GpuDx12() override;

		void Close() override;

		GpuMemoryUsageInfo GetMemoryUsageInfo() const override;

		UniquePtr<ICommandPool> CreateGraphicsCommandPool() override;
		UniquePtr<ICommandPool> CreateComputeCommandPool() override;
		std::optional<UniquePtr<ICommandPool>> CreateTransferOnlyCommandPool() override;
		UniquePtr<IGpuImageSampler> CreateSampler(const GpuImageSamplerDesc& info) const override;

		static ComPtr<IDXGIAdapter4> ChooseDeviceAdapter(ComPtr<IDXGIFactory4> factory);
		void CreateDevice(ComPtr<IDXGIAdapter4> adapter, bool useDebug);

		IDXGIAdapter1* GetAdapter() const;
		ID3D12Device* GetDevice() const;

	private:

		ComPtr<IDXGIAdapter4> m_adapter;
		ComPtr<ID3D12Device5> m_device;

	};

}

#endif
