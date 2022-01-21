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

		void BeginRenderpass(IRenderpass* renderpass) override;
		void BeginAndClearRenderpass(IRenderpass* renderpass, const Color& color) override;
		void EndRenderpass(IRenderpass* renderpass) override;

		void TransitionImageLayout(GpuImage* image, GpuImageLayout previous, GpuImageLayout next) override;

		void BindPipeline(IGraphicsPipeline* pipeline) override;
		void BindVertexBuffer(IGpuVertexBuffer* buffer) override;
		void BindIndexBuffer(IGpuIndexBuffer* buffer) override;

		/// <summary>
		/// En ocasiones será necesario cambiar el estado de un recurso para hacer
		/// operaciones con él.
		/// </summary>
		/// <param name="resource">Recurso al que se le va a cambiar su estado.</param>
		/// <param name="from">Estado actual del recurso.</param>
		/// <param name="to">Estado necesario del recurso.</param>
		void ResourceBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to);

		void SetViewport(const Viewport& viewport) override;
		void SetScissor(const Vector4ui& scissor) override;

	private:

		ComPtr<ID3D12GraphicsCommandList> commandList;
		const CommandPoolDx12* commandPool;

	};

}
