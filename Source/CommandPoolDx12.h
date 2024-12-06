#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include <wrl.h>
using namespace Microsoft::WRL;
#include <d3d12.h>

#include "ICommandPool.h"

namespace OSK::GRAPHICS {

	class GpuDx12;

	class OSKAPI_CALL CommandPoolDx12 : public ICommandPool {

	public:

		CommandPoolDx12(
			GpuDx12* device,
			CommandsSupport supportedCommands,
			GpuQueueType type);

		OwnedPtr<ICommandList> CreateCommandList(const IGpu& device) override;
		OwnedPtr<ICommandList> CreateSingleTimeCommandList(const IGpu& device) override;

		ID3D12CommandAllocator* GetCommandAllocator();

		D3D12_COMMAND_LIST_TYPE GetType() const;

	private:

		D3D12_COMMAND_LIST_TYPE m_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		ComPtr<ID3D12CommandAllocator> m_commandPool;

	};

}

#endif
