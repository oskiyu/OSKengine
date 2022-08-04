#pragma once

#include "ICommandList.h"

#include "GpuUniformBufferDx12.h"

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>

namespace OSK::GRAPHICS {

	class CommandPoolDx12;
	class GpuImageDx12;

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

		void BeginGraphicsRenderpass(DynamicArray<RenderPassImageInfo> colorImages, RenderPassImageInfo depthImage, const Color& color);
		void EndGraphicsRenderpass() override;

		void TransitionImageLayout(GpuImage* image, GpuImageLayout previous, GpuImageLayout next, TSize baseLayer, TSize numLayers, TSize baseMipLevel, TSize numMipLevels) override;

		void BindMaterial(Material* material) override;
		void BindVertexBuffer(const IGpuVertexBuffer* buffer) override;
		void BindIndexBuffer(const IGpuIndexBuffer* buffer) override;
		void BindMaterialSlot(const IMaterialSlot* slot) override;
		void PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size, TSize offset) override;

		void DrawSingleInstance(TSize numIndices) override;
		void DrawSingleMesh(TSize firstIndex, TSize numIndices) override;
		void TraceRays(TSize raygenEntry, TSize closestHitEntry, TSize missEntry, const Vector2ui& resolution) override;

		void DispatchCompute(const Vector3ui& groupCount) override;
		void BindComputePipeline(const IComputePipeline& computePipeline) override;

		void CopyBufferToImage(const GpuDataBuffer* source, GpuImage* dest, TSize layer, TSize offset) override;
		void CopyImageToImage(const GpuImage* source, GpuImage* destination, TSize numLayers, TSize srcStartLayer, TSize dstStartLayer, TSize srcMipLevel, TSize dstMipLevel, Vector2ui copySize) override;

		void BindUniformBufferDx12(TSize index, const GpuUniformBufferDx12* buffer);
		void BindImageDx12(TSize index, const GpuImageDx12* image);

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
