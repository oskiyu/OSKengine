#pragma once

#include "ICommandList.h"

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>

namespace OSK {

	class CommandPoolDx12;

	/// <summary>
	/// Una lista de comandos contiene una serie de comandos que serán
	/// enviados a la GPU para su ejecución.
	/// La lista es creada por una pool de comandos, y se introduce
	/// en una cola de comandos para su ejecución.
	/// 
	/// Esta es la implementación de la lista de comandos para el 
	/// renderizador de DirectX 12.
	/// </summary>
	class OSKAPI_CALL CommandListDx12 : public ICommandList {

	public:

		void SetCommandPool(const CommandPoolDx12& commandPool);

		void SetCommandList(const ComPtr<ID3D12GraphicsCommandList>& commandList);
		ID3D12GraphicsCommandList* GetCommandList() const;

		void Reset() override;
		void Start() override;
		void Close() override;

	private:

		ComPtr<ID3D12GraphicsCommandList> commandList;
		const CommandPoolDx12* commandPool;

	};

}
