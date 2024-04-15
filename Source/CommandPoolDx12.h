#pragma once

#include <wrl.h>
using namespace Microsoft::WRL;
#include <d3d12.h>

#include "ICommandPool.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Una pool de comandos se encarga de crear una serie de listas de comandos.
	/// 
	/// Esta es la implementación del pool para el renderizador de DirectX 12.
	/// </summary>
	class OSKAPI_CALL CommandPoolDx12 : public ICommandPool {

	public:

		CommandPoolDx12(
			CommandsSupport supportedCommands,
			GpuQueueType type);

		/// <summary>
		/// Crea una nueva lista de comandos.
		/// </summary>
		OwnedPtr<ICommandList> CreateCommandList(const IGpu& device) override;
		OwnedPtr<ICommandList> CreateSingleTimeCommandList(const IGpu& device) override;

		void SetCommandPool(const ComPtr<ID3D12CommandAllocator>& commandPool);
		ID3D12CommandAllocator* GetCommandAllocator() const;

	private:

		ComPtr<ID3D12CommandAllocator> commandPool;

	};

}
