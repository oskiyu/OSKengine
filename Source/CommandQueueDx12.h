#pragma once

#include <wrl.h>
#include <d3d12.h>

#include "OSKmacros.h"
#include "ICommandQueue.h"

using namespace Microsoft::WRL;

struct ID3D12CommandQueue;

namespace OSK::GRAPHICS {

	class OSKAPI_CALL CommandQueueDx12 : public ICommandQueue {

	public:

		CommandQueueDx12(
			const QueueFamily& family,
			UIndex32 indexInsideFamily,
			GpuQueueType type);

		void SetCommandQueue(const ComPtr<ID3D12CommandQueue>& commandQueue);
		ID3D12CommandQueue* GetCommandQueue() const;

	private:

		ComPtr<ID3D12CommandQueue> commandQueue;

	};

}
