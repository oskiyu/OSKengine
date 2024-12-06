#pragma once

#include "Platforms.h"
#ifdef OSK_USE_DIRECTX12_BACKEND

#include "ICommandList.h"

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>

namespace OSK::GRAPHICS {

	class CommandPoolDx12;
	class GpuImageDx12;
	class GpuDx12;

	class OSKAPI_CALL CommandListDx12 final : public ICommandList {

	public:

		CommandListDx12(
			const GpuDx12& gpu,
			CommandPoolDx12* commandPool);
		CommandListDx12() = default;

		ID3D12GraphicsCommandList* GetCommandList() const;

		void Reset() override;
		void Start() override;
		void Close() override;

		void ClearImage(
			GpuImage* image,
			const GpuImageRange& range,
			const Color& color) override;

		void BeginGraphicsRenderpass(DynamicArray<RenderPassImageInfo> colorImages, RenderPassImageInfo depthImage, const Color& color, bool autoSync) override;
		void EndGraphicsRenderpass(bool autoSync) override;

		void SetGpuImageBarrier(
			GpuImage* image, 
			GpuImageLayout previousLayout, 
			GpuImageLayout nextLayout, 
			GpuBarrierInfo previous, 
			GpuBarrierInfo next, 
			const GpuImageRange& range, 
			const ResourceQueueTransferInfo queueTranfer) override;
		void SetGpuBufferBarrier(
			GpuBuffer* buffer,
			const GpuBufferRange& range,
			GpuBarrierInfo previous,
			GpuBarrierInfo next,
			const ResourceQueueTransferInfo queueTranfer) override;

		void BindVertexBufferRange(const GpuBuffer& buffer, const VertexBufferView& view) override;
		void BindIndexBufferRange(const GpuBuffer& buffer, const IndexBufferView& view) override;
		void BindMaterialSlot(const IMaterialSlot& slot) override;
		void PushMaterialConstants(const std::string& pushConstName, const void* data, USize32 size, USize32 offset) override;

		void DrawSingleInstance(USize32 numIndices) override;
		void DrawSingleMesh(UIndex32 firstIndex, USize32 numIndices) override;
		void DrawInstances(UIndex32 firstIndex, USize32 numIndices, UIndex32 firstInstance, USize32 instanceCount) override;
		void TraceRays(UIndex32 raygenEntry, UIndex32 closestHitEntry, UIndex32 missEntry, const Vector2ui& resolution) override;

		void DrawMeshShader(const Vector3ui& groupCount) override {};
		void DispatchCompute(const Vector3ui& groupCount) override;

		void CopyBufferToImage(const GpuBuffer& source, GpuImage* dest, UIndex32 layer, USize64 offset) override;
		void RawCopyImageToImage(const GpuImage& source, GpuImage* destination, const CopyImageInfo& copyInfo) override;
		void CopyImageToImage(const GpuImage& source, GpuImage* destination, const CopyImageInfo& copyInfo, GpuImageFilteringType filter) override;
		void CopyBufferToBuffer(const GpuBuffer& source, GpuBuffer* dest, USize64 size, USize64 sourceOffset, USize64 destOffset) override;

		void BindUniformBufferDx12(UIndex32 index, const GpuBuffer* buffer);

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

		void SetDebugName(const std::string& name) override;

		void AddDebugMarker(const std::string& mark, const Color& color) override;
		void StartDebugSection(const std::string& mark, const Color& color) override;
		void EndDebugSection() override;

	private:

		static DXGI_FORMAT GetIndexFormat(IndexType type);

		void BindGraphicsPipeline(const IGraphicsPipeline& computePipeline) override;
		void BindComputePipeline(const IComputePipeline& computePipeline) override;
		void BindRayTracingPipeline(const IRaytracingPipeline& computePipeline) override;
		void BindMeshPipeline(const IMeshPipeline& pipeline) override;

		ComPtr<ID3D12GraphicsCommandList> commandList;

	};

}

#endif
