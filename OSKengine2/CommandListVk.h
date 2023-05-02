#pragma once

#include "ICommandList.h"
#include "DynamicArray.hpp"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class GpuBuffer;
	class GraphicsPipelineVk;


	class OSKAPI_CALL CommandListVk final : public ICommandList {

	public:

		const DynamicArray<VkCommandBuffer>& GetCommandBuffers() const;
		DynamicArray<VkCommandBuffer>* GetCommandBuffers();

		void Reset() override;
		void Start() override;
		void Close() override;

		void ClearImage(
			GpuImage* image,
			const GpuImageRange& range,
			const Color& color) override;

		void BeginGraphicsRenderpass(
			DynamicArray<RenderPassImageInfo> colorImages, 
			RenderPassImageInfo depthImage, 
			const Color& color, 
			bool autoSync) override;
		void EndGraphicsRenderpass(bool autoSync) override;

		void BindVertexBufferRange(const GpuBuffer& buffer, const VertexBufferView& view) override;
		void BindIndexBufferRange(const GpuBuffer& buffer, const IndexBufferView& view) override;
		void BindMaterialSlot(const IMaterialSlot& slot) override;
		void PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size, TSize offset) override;

		void DrawSingleInstance(TSize numIndices) override;
		void DrawSingleMesh(TSize firstIndex, TSize numIndices) override;
		void TraceRays(TSize raygenEntry, TSize closestHitEntry, TSize missEntry, const Vector2ui& resolution) override;

		void DispatchCompute(const Vector3ui& groupCount);

		void SetGpuImageBarrier(GpuImage* image, GpuImageLayout previousLayout, GpuImageLayout nextLayout, GpuBarrierInfo previous, GpuBarrierInfo next, const GpuImageRange& range) override;

		void CopyBufferToImage(const GpuBuffer& source, GpuImage* dest, TSize layer, TSize offset) override;
		void RawCopyImageToImage(const GpuImage& source, GpuImage* destination, const CopyImageInfo& copyInfo) override;
		void CopyBufferToBuffer(const GpuBuffer& source, GpuBuffer* dest, TSize size, TSize sourceOffset, TSize destOffset) override;

		void SetViewport(const Viewport& viewport) override;
		void SetScissor(const Vector4ui& scissor) override;

		void SetDebugName(const std::string& name) override;

		void AddDebugMarker(const std::string& mark, const Color& color) override;
		void StartDebugSection(const std::string& mark, const Color& color) override;
		void EndDebugSection() override;

	private:

		static VkIndexType GetIndexType(IndexType type);

		void BindGraphicsPipeline(const IGraphicsPipeline& computePipeline) override;
		void BindComputePipeline(const IComputePipeline& computePipeline) override;
		void BindRayTracingPipeline(const IRaytracingPipeline& computePipeline) override;

		VkPipelineStageFlagBits2 GetPipelineStage(GpuCommandStage stage) const;
		VkAccessFlags2 GetPipelineAccess(GpuAccessStage stage) const;

		/// <summary>
		/// Varias listas nativas, una por cada imagen en el swapchain.
		/// </summary>
		DynamicArray<VkCommandBuffer> commandBuffers;

	};

}
