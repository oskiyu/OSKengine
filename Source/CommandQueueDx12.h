#pragma once

#include <wrl.h>
#include <d3d12.h>

#include "OSKmacros.h"
#include "ICommandQueue.h"

using namespace Microsoft::WRL;

struct ID3D12CommandQueue;

namespace OSK::GRAPHICS {

	/// <summary>
	/// Una cola de comandos almacena todas las listas de comandos que se envían a la GPU.
	/// 
	/// Implementación de la interfaz para el renderizador de DirectX 12.
	/// </summary>
	class OSKAPI_CALL CommandQueueDx12 : public ICommandQueue {

	public:

		CommandQueueDx12(CommandQueueSupport support);

		void SetCommandQueue(const ComPtr<ID3D12CommandQueue>& commandQueue);
		ID3D12CommandQueue* GetCommandQueue() const;

	private:

		ComPtr<ID3D12CommandQueue> commandQueue;

	};

}
