#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include <wrl.h>
#include <d3d12.h>

#include "ApiCall.h"
#include "ICommandQueue.h"

using namespace Microsoft::WRL;

struct ID3D12CommandQueue;

namespace OSK::GRAPHICS {

	class GpuDx12;

	class OSKAPI_CALL CommandQueueDx12 : public ICommandQueue {

	public:

		CommandQueueDx12(
			GpuDx12* device,
			GpuQueueType type);

		ID3D12CommandQueue* GetCommandQueue();

	private:

		ComPtr<ID3D12CommandQueue> m_commandQueue;

	};

}

#endif
