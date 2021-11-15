#pragma once

#include "ISyncDevice.h"

#include <wrl.h>
#include <d3d12.h>
using namespace Microsoft::WRL;

namespace OSK {

	class GpuDx12;
	class CommandQueueDx12;

	class OSKAPI_CALL SyncDeviceDx12 : public ISyncDevice {

	public:

		void SetFenceEvent(HANDLE fenceEvent);
		void SetFence(const ComPtr<ID3D12Fence>& fence);

		void Await();
		void Flush(const CommandQueueDx12& commandList);

	private:

		UINT64 fenceValue;
		UINT64 previousFenceValue;
		HANDLE fenceEvent;
		ComPtr<ID3D12Fence> fence;

	};


}
