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

		/// <summary>
		/// Establece el evento que será llamado para avisar al dispositivo
		/// de sincronización.
		/// </summary>
		void SetFenceEvent(HANDLE fenceEvent);

		/// <summary>
		/// Establece el dispositivo nativo de sincronización.
		/// </summary>
		void SetFence(const ComPtr<ID3D12Fence>& fence);

		/// <summary>
		/// Pone en espera el hilo actual, hasta que se pueda renderizar
		/// en la nueva imagen del swapchain.
		/// </summary>
		void Await();

		/// <summary>
		/// Una vez se han grabado todos los comandos, la lista es enviada a
		/// la GPU para su ejecución.
		/// </summary>
		void Flush(const CommandQueueDx12& commandList);

	private:

		UINT64 fenceValue;
		UINT64 previousFenceValue;
		HANDLE fenceEvent;
		ComPtr<ID3D12Fence> fence;

	};


}
