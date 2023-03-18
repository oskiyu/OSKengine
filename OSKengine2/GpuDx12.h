#pragma once

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>
#include <dxgi1_6.h>

#include "OSKmacros.h"
#include "IGpu.h"
#include "UniquePtr.hpp"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuDx12 final : public IGpu {

	public:

		~GpuDx12();
		void Close() override;

		GpuMemoryUsageInfo GetMemoryUsageInfo() const override;

		OwnedPtr<ICommandPool> CreateGraphicsCommandPool() override;
		OwnedPtr<ICommandPool> CreateComputeCommandPool() override;

		OwnedPtr<ISyncDevice> CreateSyncDevice() override;

		void SetAdapter(const ComPtr<IDXGIAdapter1>& adapter);
		void SetDevice(const ComPtr<ID3D12Device>& device);

		IDXGIAdapter1* GetAdapter() const;
		ID3D12Device* GetDevice() const;

	private:

		ComPtr<IDXGIAdapter1> adapter;
		ComPtr<ID3D12Device> device;

	};

}
