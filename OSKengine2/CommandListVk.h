#pragma once

#include "ICommandList.h"
#include "DynamicArray.hpp"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class GpuBuffer;
	class GraphicsPipelineVk;


	class OSKAPI_CALL CommandListVk final : public ICommandList {

	public:

		CommandListVk() = default;

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
		void PushMaterialConstants(const std::string& pushConstName, const void* data, USize32 size, USize32 offset) override;

		void DrawSingleInstance(USize32 numIndices) override;
		void DrawSingleMesh(UIndex32 firstIndex, USize32 numIndices) override;
		void TraceRays(UIndex32 raygenEntry, UIndex32 closestHitEntry, UIndex32 missEntry, const Vector2ui& resolution) override;

		void DispatchCompute(const Vector3ui& groupCount);

		void SetGpuImageBarrier(GpuImage* image, GpuImageLayout previousLayout, GpuImageLayout nextLayout, GpuBarrierInfo previous, GpuBarrierInfo next, const GpuImageRange& range) override;

		void CopyBufferToImage(const GpuBuffer& source, GpuImage* dest, UIndex32 layer, USize64 offset) override;
		void RawCopyImageToImage(const GpuImage& source, GpuImage* destination, const CopyImageInfo& copyInfo) override;
		void CopyBufferToBuffer(const GpuBuffer& source, GpuBuffer* dest, USize64 size, USize64 sourceOffset, USize64 destOffset) override;

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
