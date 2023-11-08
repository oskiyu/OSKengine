#include "CommandListVk.h"

#include <vulkan/vulkan.h>
#include "GpuImageLayoutVk.h"
#include "GpuImageLayout.h"
#include "GpuMemorySubblockVk.h"
#include "GpuMemoryBlockVk.h"
#include "GpuImageVk.h"
#include "Color.hpp"
#include "RenderpassType.h"
#include "GpuBuffer.h"
#include "Viewport.h"
#include "GraphicsPipelineVk.h"
#include "GpuBuffer.h"
#include "MaterialSlotVk.h"
#include "GraphicsPipelineVk.h"
#include "RaytracingPipelineVk.h"
#include "PipelineLayoutVk.h"
#include "Material.h"
#include "MaterialLayout.h"
#include "MaterialLayoutSlot.h"
#include "ShaderBindingTypeVk.h"
#include "OSKengine.h"
#include "RendererVk.h"
#include "RtShaderTableVk.h"
#include "ComputePipelineVk.h"
#include "GpuVk.h"
#include "GpuImageViewVk.h"
#include "GpuImageVk.h"
#include "IGpuImage.h"
#include "GpuImageVk.h"

#include "CommandListExceptions.h"
#include "NotImplementedException.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

const DynamicArray<VkCommandBuffer>& CommandListVk::GetCommandBuffers() const {
	return commandBuffers;
}

DynamicArray<VkCommandBuffer>* CommandListVk::GetCommandBuffers() {
	return &commandBuffers;
}

void CommandListVk::Reset() {
	VkResult result = vkResetCommandBuffer(commandBuffers[_GetCommandListIndex()], 0);
	OSK_ASSERT(result == VK_SUCCESS, CommandListResetException(result));
}

void CommandListVk::Start() {
	const VkCommandBufferBeginInfo beginInfo {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		NULL,
		0, // flags
		NULL
	};

	VkResult result = vkBeginCommandBuffer(commandBuffers[_GetCommandListIndex()], &beginInfo);
	OSK_ASSERT(result == VK_SUCCESS, CommandListStartException(result));
}

void CommandListVk::Close() {
	VkResult result = vkEndCommandBuffer(commandBuffers[_GetCommandListIndex()]);
	OSK_ASSERT(result == VK_SUCCESS, CommandListEndException(result));
}

void CommandListVk::SetGpuImageBarrier(GpuImage* image, GpuImageLayout previousLayout, GpuImageLayout nextLayout, GpuBarrierInfo previous, GpuBarrierInfo next, const GpuImageRange& imageInfo) {
	VkImageMemoryBarrier2 barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;

	barrier.oldLayout = GetGpuImageLayoutVk(previousLayout);
	barrier.newLayout = GetGpuImageLayoutVk(nextLayout);

	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.image = image->As<GpuImageVk>()->GetVkImage();

	barrier.subresourceRange.baseMipLevel = imageInfo.baseMipLevel;
	barrier.subresourceRange.levelCount = imageInfo.numMipLevels;
	barrier.subresourceRange.baseArrayLayer = imageInfo.baseLayer;
	barrier.subresourceRange.layerCount = imageInfo.numLayers;

	barrier.subresourceRange.aspectMask = 0;
	if (EFTraits::HasFlag(imageInfo.channel, SampledChannel::COLOR))
		barrier.subresourceRange.aspectMask = barrier.subresourceRange.aspectMask | VK_IMAGE_ASPECT_COLOR_BIT;
	if (EFTraits::HasFlag(imageInfo.channel, SampledChannel::DEPTH))
		barrier.subresourceRange.aspectMask = barrier.subresourceRange.aspectMask | VK_IMAGE_ASPECT_DEPTH_BIT;
	if (EFTraits::HasFlag(imageInfo.channel, SampledChannel::STENCIL))
		barrier.subresourceRange.aspectMask = barrier.subresourceRange.aspectMask | VK_IMAGE_ASPECT_STENCIL_BIT;

	barrier.srcAccessMask = GetPipelineAccess(previous.accessStage);
	barrier.dstAccessMask = GetPipelineAccess(next.accessStage);

	barrier.srcStageMask = GetPipelineStage(previous.stage);
	barrier.dstStageMask = GetPipelineStage(next.stage);
	
	VkDependencyInfo info{};
	info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;

	info.imageMemoryBarrierCount = 1;
	info.pImageMemoryBarriers = &barrier;
	info.dependencyFlags = 0;

	vkCmdPipelineBarrier2(commandBuffers[_GetCommandListIndex()], &info);

	image->SetCurrentBarrier(next);
	image->_SetLayout(
		imageInfo.baseLayer,
		imageInfo.numLayers,
		imageInfo.baseMipLevel,
		imageInfo.numMipLevels,
		nextLayout);
}

void CommandListVk::CopyBufferToImage(const GpuBuffer& source, GpuImage* dest, UIndex32 layer, USize64 offset) {
	VkBufferImageCopy region{};

	region.bufferOffset = source.GetMemorySubblock()->GetOffsetFromBlock() + offset;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = layer;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		dest->GetSize2D().x,
		dest->GetSize2D().y,
		1
	};

	// Layout[0] = TRANSFER_DEST
	vkCmdCopyBufferToImage(
		commandBuffers[_GetCommandListIndex()],
		source.GetMemoryBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(), 
		dest->As<GpuImageVk>()->GetVkImage(), 
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	if (dest->GetMipLevels() > 1) {
		// Mip levels
		Vector2i mipSize = dest->GetSize2D().ToVector2i();
		// El nivel 0 ya está lleno.
		for (UIndex32 mipLevel = 1; mipLevel < dest->GetMipLevels(); mipLevel++) {

			// Mip anterior: layout DESTINATION -> SOURCE
			SetGpuImageBarrier(
				dest,
				GpuImageLayout::TRANSFER_DESTINATION,
				GpuImageLayout::TRANSFER_SOURCE,
				GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE),
				GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_READ),
				{ .baseLayer = layer, .numLayers = 1, .baseMipLevel = mipLevel - 1, .numMipLevels = 1 });

			VkImageBlit blit{};

			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipSize.x, mipSize.y, 1 };

			// Origen: mip level anterior.
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = mipLevel - 1;
			blit.srcSubresource.baseArrayLayer = layer;
			blit.srcSubresource.layerCount = 1;

			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipSize.x > 1 ? mipSize.x / 2 : 1, mipSize.y > 1 ? mipSize.y / 2 : 1, 1 };

			// Destino.
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = mipLevel;
			blit.dstSubresource.baseArrayLayer = layer;
			blit.dstSubresource.layerCount = 1;

			// Mip actual: layout DESTINATION
			vkCmdBlitImage(commandBuffers[_GetCommandListIndex()],
				dest->As<GpuImageVk>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				dest->As<GpuImageVk>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit, VK_FILTER_LINEAR);

			if (mipSize.x > 1)
				mipSize.x /= 2;
			if (mipSize.y > 1)
				mipSize.y /= 2;
		}

		// Establecer layout del último nivel = TRANSFER_SOURCE
		SetGpuImageBarrier(
			dest,
			GpuImageLayout::TRANSFER_DESTINATION,
			GpuImageLayout::TRANSFER_SOURCE,
			GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE),
			GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_READ),
			{ .baseLayer = layer, .numLayers = 1, .baseMipLevel = dest->GetMipLevels() - 1, .numMipLevels = 1 });

		// Establecer todo TRANSFER_SOURCE -> TRANSFER_DESTINATION
		SetGpuImageBarrier(
			dest,
			GpuImageLayout::TRANSFER_SOURCE,
			GpuImageLayout::TRANSFER_DESTINATION,
			GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_READ),
			GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE),
			{ .baseLayer = layer, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
	}
}

void CommandListVk::RawCopyImageToImage(const GpuImage& source, GpuImage* destination, const CopyImageInfo& copyInfo) {
	VkImageCopy region{};

	region.srcOffset = {
		static_cast<int>(copyInfo.sourceOffset.x),
		static_cast<int>(copyInfo.sourceOffset.y),
		static_cast<int>(copyInfo.sourceOffset.z)
	};
	region.dstOffset = {
		static_cast<int>(copyInfo.destinationOffset.x),
		static_cast<int>(copyInfo.destinationOffset.y),
		static_cast<int>(copyInfo.destinationOffset.z)
	};

	const USize32 numLayers = copyInfo.numArrayLevels == CopyImageInfo::ALL_ARRAY_LEVELS ? source.GetNumLayers() : copyInfo.numArrayLevels;

	region.srcSubresource.aspectMask = GpuImageVk::GetAspectFlags(copyInfo.sourceChannel);
	region.srcSubresource.mipLevel = copyInfo.sourceMipLevel;
	region.srcSubresource.baseArrayLayer = copyInfo.sourceArrayLevel;
	region.srcSubresource.layerCount = numLayers;

	region.dstSubresource.aspectMask = GpuImageVk::GetAspectFlags(copyInfo.destinationChannel);
	region.dstSubresource.mipLevel = copyInfo.destinationMipLevel;
	region.dstSubresource.baseArrayLayer = copyInfo.destinationArrayLevel;
	region.dstSubresource.layerCount = numLayers;

	region.extent.width = copyInfo.copySize.x;
	region.extent.height= copyInfo.copySize.y;
	region.extent.depth = copyInfo.copySize.z;

	vkCmdCopyImage(commandBuffers[_GetCommandListIndex()],
		source.As<GpuImageVk>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		destination->As<GpuImageVk>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1, &region);
}

void CommandListVk::CopyImageToImage(const GpuImage& source, GpuImage* destination, const CopyImageInfo& copyInfo, GpuImageFilteringType filter) {
	const USize32 numLayers = copyInfo.numArrayLevels == CopyImageInfo::ALL_ARRAY_LEVELS ? source.GetNumLayers() : copyInfo.numArrayLevels; 
	
	VkImageBlit copyRegion{};

	copyRegion.srcOffsets[0] = {
		static_cast<int>(copyInfo.sourceOffset.x),
		static_cast<int>(copyInfo.sourceOffset.y),
		static_cast<int>(copyInfo.sourceOffset.z)
	};
	copyRegion.srcOffsets[1] = {
		static_cast<int>(copyInfo.sourceOffset.x),
		static_cast<int>(copyInfo.sourceOffset.y),
		static_cast<int>(copyInfo.sourceOffset.z)
	};

	copyRegion.dstOffsets[0] = {
		static_cast<int>(copyInfo.destinationOffset.x),
		static_cast<int>(copyInfo.destinationOffset.y),
		static_cast<int>(copyInfo.destinationOffset.z)
	};
	copyRegion.dstOffsets[1] = {
		static_cast<int>(copyInfo.destinationOffset.x),
		static_cast<int>(copyInfo.destinationOffset.y),
		static_cast<int>(copyInfo.destinationOffset.z)
	};

	VkImageSubresourceLayers sourceLayer{};
	sourceLayer.aspectMask = GpuImageVk::GetAspectFlags(copyInfo.sourceChannel);
	sourceLayer.baseArrayLayer = copyInfo.sourceArrayLevel;
	sourceLayer.layerCount = numLayers;
	sourceLayer.mipLevel = copyInfo.sourceMipLevel;

	copyRegion.srcSubresource = sourceLayer;

	VkImageSubresourceLayers destinationLayer{};
	destinationLayer.aspectMask = GpuImageVk::GetAspectFlags(copyInfo.destinationChannel);
	destinationLayer.baseArrayLayer = copyInfo.destinationArrayLevel;
	destinationLayer.layerCount = numLayers;
	destinationLayer.mipLevel = copyInfo.destinationMipLevel;

	copyRegion.dstSubresource = destinationLayer;

	vkCmdBlitImage(
		commandBuffers[_GetCommandListIndex()],
		source.As<GpuImageVk>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		destination->As<GpuImageVk>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1, &copyRegion,
		GpuImageVk::GetFilterTypeVulkan(filter));
}

void CommandListVk::CopyBufferToBuffer(const GpuBuffer& source, GpuBuffer* dest, USize64 size, USize64 sourceOffset, USize64 destOffset) {
	VkBufferCopy copyRegion{};

	copyRegion.srcOffset = sourceOffset + source.GetMemorySubblock()->GetOffsetFromBlock();
	copyRegion.dstOffset = destOffset + dest->GetMemorySubblock()->GetOffsetFromBlock();
	copyRegion.size = size;

	vkCmdCopyBuffer(commandBuffers[_GetCommandListIndex()],
		source.GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(),
		dest->GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(), 
		1, &copyRegion);
}

void CommandListVk::ClearImage(GpuImage* image, const GpuImageRange& range, const Color& color) {
	VkClearColorValue vkColor{};
	vkColor.float32[0] = color.red;
	vkColor.float32[1] = color.green;
	vkColor.float32[2] = color.blue;
	vkColor.float32[3] = color.alpha;

	VkImageSubresourceRange vkRange{};
	vkRange.baseArrayLayer = range.baseLayer;
	vkRange.layerCount = range.numLayers;
	vkRange.baseMipLevel = range.baseMipLevel;
	vkRange.levelCount = range.numMipLevels;

	vkRange.aspectMask = 0;
	if (EFTraits::HasFlag(range.channel, SampledChannel::COLOR))
		vkRange.aspectMask = vkRange.aspectMask | VK_IMAGE_ASPECT_COLOR_BIT;
	if (EFTraits::HasFlag(range.channel, SampledChannel::DEPTH))
		vkRange.aspectMask = vkRange.aspectMask | VK_IMAGE_ASPECT_DEPTH_BIT;
	if (EFTraits::HasFlag(range.channel, SampledChannel::STENCIL))
		vkRange.aspectMask = vkRange.aspectMask | VK_IMAGE_ASPECT_STENCIL_BIT;

	vkCmdClearColorImage(
		commandBuffers[_GetCommandListIndex()], 
		image->As<GpuImageVk>()->GetVkImage(), 
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
		&vkColor, 1, &vkRange);
}

void CommandListVk::BeginGraphicsRenderpass(DynamicArray<RenderPassImageInfo> colorImages, RenderPassImageInfo depthImage, const Color& color, bool autoSync) {
	const Vector2ui targetSize = colorImages[0].targetImage->GetSize2D();

	if (autoSync) {
		for (const auto& img : colorImages) {
			SetGpuImageBarrier(
				img.targetImage,
				GpuImageLayout::UNDEFINED, 
				GpuImageLayout::COLOR_ATTACHMENT,
				GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
				GpuBarrierInfo(GpuCommandStage::COLOR_ATTACHMENT_OUTPUT, GpuAccessStage::COLOR_ATTACHMENT_WRITE),
				{ .baseLayer = img.arrayLevel,
					.numLayers = 1,
					.baseMipLevel = 0,
					.numMipLevels = ALL_MIP_LEVELS, });
		}

		SampledChannel depthChannel = SampledChannel::DEPTH;
		if (FormatSupportsStencil(depthImage.targetImage->GetFormat()))
			depthChannel |= SampledChannel::STENCIL;

		SetGpuImageBarrier(
			depthImage.targetImage,
			GpuImageLayout::UNDEFINED,
			GpuImageLayout::DEPTH_STENCIL_TARGET,
			GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
			GpuBarrierInfo(GpuCommandStage::DEPTH_STENCIL_START, GpuAccessStage::DEPTH_STENCIL_READ | GpuAccessStage::DEPTH_STENCIL_WRITE),
			{ .baseLayer = depthImage.arrayLevel,
				.numLayers = 1,
				.baseMipLevel = 0,
				.numMipLevels = ALL_MIP_LEVELS,
				.channel = depthChannel });
	}

	GpuImageViewConfig colorAttachmentConfig = GpuImageViewConfig::CreateTarget_Color();
	DynamicArray<VkRenderingAttachmentInfo> colorAttachments = DynamicArray<VkRenderingAttachmentInfo>::CreateResizedArray(colorImages.GetSize());
	for (UIndex64 i = 0; i < colorImages.GetSize(); i++) {
		colorAttachmentConfig.baseArrayLevel = colorImages[i].arrayLevel;

		colorAttachments[i] = {};
		colorAttachments[i].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		colorAttachments[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachments[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachments[i].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachments[i].imageView = colorImages[i].targetImage->GetView(colorAttachmentConfig)->As<GpuImageViewVk>()->GetVkView();
		colorAttachments[i].resolveMode = VK_RESOLVE_MODE_NONE;
		colorAttachments[i].resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachments[i].resolveImageView = VK_NULL_HANDLE;
		colorAttachments[i].clearValue.color = { color.red, color.green, color.blue, color.alpha };
	}

	GpuImageViewConfig dephtAttachmentConfig = FormatSupportsStencil(depthImage.targetImage->GetFormat())
		? GpuImageViewConfig::CreateTarget_DepthStencil()
		: GpuImageViewConfig::CreateTarget_Depth();
	dephtAttachmentConfig.baseArrayLevel = depthImage.arrayLevel;

	VkRenderingAttachmentInfo depthAttachment{};
	depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.imageLayout = FormatSupportsStencil(depthImage.targetImage->GetFormat()) 
		? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		: VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
	depthAttachment.imageView = depthImage.targetImage->GetView(dephtAttachmentConfig)->As<GpuImageViewVk>()->GetVkView();
	depthAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
	depthAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.resolveImageView = VK_NULL_HANDLE;
	depthAttachment.clearValue.depthStencil = { 0.0f, 0 };

	VkRenderingInfo renderpassInfo{};
	renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderpassInfo.renderArea = { 0, 0, targetSize.x, targetSize.y };
	renderpassInfo.layerCount = 1;
	renderpassInfo.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.GetSize());
	renderpassInfo.pColorAttachments = colorAttachments.GetData();
	renderpassInfo.pDepthAttachment = &depthAttachment;
	renderpassInfo.pStencilAttachment = FormatSupportsStencil(depthImage.targetImage->GetFormat()) 
		? &depthAttachment
		: VK_NULL_HANDLE;

	RendererVk::pvkCmdBeginRendering(commandBuffers.At(_GetCommandListIndex()), &renderpassInfo);

	currentColorImages = colorImages;
	currentDepthImage = depthImage;
}

void CommandListVk::EndGraphicsRenderpass(bool autoSync) {
	RendererVk::pvkCmdEndRendering(commandBuffers[_GetCommandListIndex()]);

	const bool isFinal = currentRenderpassType == RenderpassType::FINAL;

	if (currentRenderpassType != RenderpassType::INTERMEDIATE)
		currentRenderpassType = RenderpassType::INTERMEDIATE;
}

void CommandListVk::BindVertexBufferRange(const GpuBuffer& buffer, const VertexBufferView& view) {
	VkBuffer vertexBuffer = buffer.GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer();
	VkDeviceSize offset = buffer.GetMemorySubblock()->GetOffsetFromBlock() + view.offsetInBytes;

	vkCmdBindVertexBuffers(commandBuffers[_GetCommandListIndex()], 0, 1, &vertexBuffer, &offset);
}

void CommandListVk::BindIndexBufferRange(const GpuBuffer& buffer, const IndexBufferView& view) {
	vkCmdBindIndexBuffer(
		commandBuffers[_GetCommandListIndex()],
		buffer.GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(), 
		buffer.GetMemorySubblock()->GetOffsetFromBlock() + view.offsetInBytes, 
		GetIndexType(view.type));
}

void CommandListVk::BindMaterialSlot(const IMaterialSlot& slot) {
	VkDescriptorSet sets[] = { slot.As<MaterialSlotVk>()->GetDescriptorSet(Engine::GetRenderer()->GetCurrentResourceIndex()) };

	VkPipelineBindPoint bindPoint{};
	switch (currentMaterial->GetMaterialType()) {
		case MaterialType::COMPUTE: bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE; break;
		case MaterialType::GRAPHICS: bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; break;
		case MaterialType::RAYTRACING: bindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR; break;
	}

	VkPipelineLayout layout = VK_NULL_HANDLE;
	switch (currentMaterial->GetMaterialType()) {
		case MaterialType::COMPUTE: layout = currentPipeline.compute->GetLayout()->As<PipelineLayoutVk>()->GetLayout(); break;
		case MaterialType::GRAPHICS: layout = currentPipeline.graphics->GetLayout()->As<PipelineLayoutVk>()->GetLayout(); break;
		case MaterialType::RAYTRACING: layout = currentPipeline.raytracing->GetLayout()->As<PipelineLayoutVk>()->GetLayout(); break;
	}

	const auto glslSetIndex = currentMaterial->GetLayout()->GetSlot(slot.GetName()).glslSetIndex;
	vkCmdBindDescriptorSets(commandBuffers[_GetCommandListIndex()], bindPoint, layout, glslSetIndex, 1, sets, 0, nullptr);
}

void CommandListVk::PushMaterialConstants(const std::string& pushConstName, const void* data, USize32 size, USize32 offset) {
	VkPipelineLayout layout = VK_NULL_HANDLE;
	switch (currentMaterial->GetMaterialType()) {
		case MaterialType::COMPUTE: layout = currentPipeline.compute->GetLayout()->As<PipelineLayoutVk>()->GetLayout(); break;
		case MaterialType::GRAPHICS: layout = currentPipeline.graphics->GetLayout()->As<PipelineLayoutVk>()->GetLayout(); break;
		case MaterialType::RAYTRACING: layout = currentPipeline.raytracing->GetLayout()->As<PipelineLayoutVk>()->GetLayout(); break;
	}
	auto& pushConstInfo = currentMaterial->GetLayout()->GetPushConstant(pushConstName);

	vkCmdPushConstants(commandBuffers[_GetCommandListIndex()], layout, GetShaderStageVk(pushConstInfo.stage), 0 + offset, size, data);
}

void CommandListVk::DrawSingleInstance(USize32 numIndices) {
	vkCmdDrawIndexed(commandBuffers[_GetCommandListIndex()], numIndices, 1, 0, 0, 0);
}

void CommandListVk::DrawSingleMesh(UIndex32 firstIndex, USize32 numIndices) {
	vkCmdDrawIndexed(commandBuffers[_GetCommandListIndex()], numIndices, 1, firstIndex, 0, 0);
}

void CommandListVk::TraceRays(UIndex32 raygenEntry, UIndex32 closestHitEntry, UIndex32 missEntry, const Vector2ui& resolution) {
	const RtShaderTableVk* shaderTable = currentPipeline.raytracing->GetShaderTable()->As<RtShaderTableVk>();

	const VkStridedDeviceAddressRegionKHR raygenTable = shaderTable->GetRaygenTableAddressRegion();
	const VkStridedDeviceAddressRegionKHR closestHitTable = shaderTable->GetClosestHitTableAddressRegion();
	const VkStridedDeviceAddressRegionKHR missTable = shaderTable->GetMissTableAddressRegion();
	const VkStridedDeviceAddressRegionKHR emptyTable{};

	RendererVk::pvkCmdTraceRaysKHR(commandBuffers[_GetCommandListIndex()],
						&raygenTable, &missTable, &closestHitTable, &emptyTable, 
						resolution.x, resolution.y, 1);
}

void CommandListVk::DispatchCompute(const Vector3ui& groupCount) {
	vkCmdDispatch(commandBuffers[_GetCommandListIndex()], groupCount.x, groupCount.y, groupCount.z);
}

void CommandListVk::BindGraphicsPipeline(const IGraphicsPipeline& pipeline) {
	vkCmdBindPipeline(commandBuffers[_GetCommandListIndex()], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.As<GraphicsPipelineVk>()->GetPipeline());
}

void CommandListVk::BindComputePipeline(const IComputePipeline& pipeline) {
	vkCmdBindPipeline(commandBuffers[_GetCommandListIndex()], VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.As<ComputePipelineVk>()->GetPipeline());
}

void CommandListVk::BindRayTracingPipeline(const IRaytracingPipeline& pipeline) {
	vkCmdBindPipeline(commandBuffers[_GetCommandListIndex()], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, pipeline.As<RaytracingPipelineVk>()->GetPipeline());
}

void CommandListVk::SetViewport(const Viewport& vp) {
	VkViewport viewport{};

	viewport.x = static_cast<float>(vp.rectangle.GetRectanglePosition().x);
	viewport.y = static_cast<float>(vp.rectangle.GetRectanglePosition().y + vp.rectangle.GetRectangleSize().y);
	viewport.width = static_cast<float>(vp.rectangle.GetRectangleSize().x);
	viewport.height = -static_cast<float>(vp.rectangle.GetRectangleSize().y);

	viewport.minDepth = vp.minDepth;
	viewport.maxDepth = vp.maxDepth;

	vkCmdSetViewport(commandBuffers[_GetCommandListIndex()], 0, 1, &viewport);
}

void CommandListVk::SetScissor(const Vector4ui& scissorRect) {
	VkRect2D scissor{};

	scissor.offset = {
		(int32_t)scissorRect.GetRectanglePosition().x,
		(int32_t)scissorRect.GetRectanglePosition().y
	};

	scissor.extent = {
		scissorRect.GetRectangleSize().x,
		scissorRect.GetRectangleSize().y
	};
	
	vkCmdSetScissor(commandBuffers[_GetCommandListIndex()], 0, 1, &scissor);
}

void CommandListVk::SetDebugName(const std::string& name) {
	VkDebugUtilsObjectNameInfoEXT nameInfo{};
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType = VK_OBJECT_TYPE_COMMAND_BUFFER;
	nameInfo.pNext = nullptr;

	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();

	for (UIndex64 i = 0; i < commandBuffers.GetSize(); i++) {
		std::string nname = name + "[" + std::to_string(i) + "]";
		nameInfo.pObjectName = nname.c_str();
		nameInfo.objectHandle = (uint64_t)commandBuffers.At(i);

		if (RendererVk::pvkSetDebugUtilsObjectNameEXT != nullptr)
			RendererVk::pvkSetDebugUtilsObjectNameEXT(logicalDevice, &nameInfo);
	}
}

void CommandListVk::AddDebugMarker(const std::string& mark, const Color& color) {
	VkDebugUtilsLabelEXT label{};
	label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
	label.pLabelName = mark.c_str();
	label.color[0] = color.red;
	label.color[1] = color.green;
	label.color[2] = color.blue;
	label.color[3] = color.alpha;
	label.pNext = nullptr;

	RendererVk::pvkCmdInsertDebugUtilsLabelEXT(commandBuffers[_GetCommandListIndex()], &label);
}

void CommandListVk::StartDebugSection(const std::string& mark, const Color& color) {
	VkDebugUtilsLabelEXT label{};
	label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
	label.pLabelName = mark.c_str();
	label.color[0] = color.red;
	label.color[1] = color.green;
	label.color[2] = color.blue;
	label.color[3] = color.alpha;
	label.pNext = nullptr;

	RendererVk::pvkCmdBeginDebugUtilsLabelEXT(commandBuffers[_GetCommandListIndex()], &label);
}

void CommandListVk::EndDebugSection() {
	RendererVk::pvkCmdEndDebugUtilsLabelEXT(commandBuffers[_GetCommandListIndex()]);
}


VkPipelineStageFlagBits2 CommandListVk::GetPipelineStage(GpuCommandStage stage) const {

	switch (stage) {
	case GpuCommandStage::VERTEX_SHADER:
		return VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
	case GpuCommandStage::FRAGMENT_SHADER:
		return VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

	case GpuCommandStage::TESSELATION_CONTROL:
		return VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT;
	case GpuCommandStage::TESSELATION_EVALUATION:
		return VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT;

	case GpuCommandStage::RAYTRACING_SHADER:
		return VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;
	case GpuCommandStage::RT_AS_BUILD:
		return VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

	case GpuCommandStage::COLOR_ATTACHMENT_OUTPUT:
		return VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	case GpuCommandStage::DEPTH_STENCIL_START:
		return VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
	case GpuCommandStage::DEPTH_STENCIL_END:
		return VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;

	case GpuCommandStage::TRANSFER:
		return VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT;

	case GpuCommandStage::COMPUTE_SHADER:
		return VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

	case GpuCommandStage::NONE:
		return VK_PIPELINE_STAGE_2_NONE;
	case GpuCommandStage::ALL:
		return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

	default:
		OSK_ASSERT(false, NotImplementedException());
		return VK_PIPELINE_STAGE_2_NONE;
	}
}

VkAccessFlags2 CommandListVk::GetPipelineAccess(GpuAccessStage stage) const {
	VkAccessFlags2 output = 0;

	if (EFTraits::HasFlag(stage, GpuAccessStage::SHADER_READ))
		output |= VK_ACCESS_2_SHADER_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuAccessStage::SHADER_WRITE))
		output |= VK_ACCESS_2_SHADER_WRITE_BIT;

	if (EFTraits::HasFlag(stage, GpuAccessStage::SAMPLED_READ))
		output |= VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuAccessStage::STORAGE_BUFFER_READ))
		output |= VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuAccessStage::STORAGE_IMAGE_READ))
		output |= VK_ACCESS_2_SHADER_STORAGE_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuAccessStage::UNIFORM_BUFFER_READ))
		output |= VK_ACCESS_2_UNIFORM_READ_BIT;

	if (EFTraits::HasFlag(stage, GpuAccessStage::COLOR_ATTACHMENT_READ))
		output |= VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuAccessStage::COLOR_ATTACHMENT_WRITE))
		output |= VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

	if (EFTraits::HasFlag(stage, GpuAccessStage::DEPTH_STENCIL_READ))
		output |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuAccessStage::DEPTH_STENCIL_WRITE))
		output |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	if (EFTraits::HasFlag(stage, GpuAccessStage::TRANSFER_READ))
		output |= VK_ACCESS_2_TRANSFER_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuAccessStage::TRANSFER_WRITE))
		output |= VK_ACCESS_2_TRANSFER_WRITE_BIT;

	if (EFTraits::HasFlag(stage, GpuAccessStage::MEMORY_READ))
		output |= VK_ACCESS_2_MEMORY_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuAccessStage::MEMORY_WRITE))
		output |= VK_ACCESS_2_MEMORY_WRITE_BIT;

	if (EFTraits::HasFlag(stage, GpuAccessStage::RT_AS_READ))
		output |= VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;
	if (EFTraits::HasFlag(stage, GpuAccessStage::RT_AS_WRITE))
		output |= VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;

	if (EFTraits::HasFlag(stage, GpuAccessStage::NONE))
		output |= VK_ACCESS_2_NONE;

	return output;
}

VkIndexType CommandListVk::GetIndexType(IndexType type) {
	switch (type)
	{
	case OSK::GRAPHICS::IndexType::U8:
		return VK_INDEX_TYPE_UINT8_EXT;
	case OSK::GRAPHICS::IndexType::U16:
		return VK_INDEX_TYPE_UINT16;
	case OSK::GRAPHICS::IndexType::U32:
		return VK_INDEX_TYPE_UINT32;
	default:
		OSK_UNREACHABLE;
		break;
	}
}
