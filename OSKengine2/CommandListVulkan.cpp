#include "CommandListVulkan.h"

#include <vulkan/vulkan.h>
#include "GpuImageLayoutVulkan.h"
#include "GpuImageLayout.h"
#include "GpuMemorySubblockVulkan.h"
#include "GpuMemoryBlockVulkan.h"
#include "GpuImageVulkan.h"
#include "Color.hpp"
#include "RenderpassType.h"
#include "IGpuDataBuffer.h"
#include "Viewport.h"
#include "GraphicsPipelineVulkan.h"
#include "GpuVertexBufferVulkan.h"
#include "GpuIndexBufferVulkan.h"
#include "MaterialSlotVulkan.h"
#include "GraphicsPipelineVulkan.h"
#include "RaytracingPipelineVulkan.h"
#include "PipelineLayoutVulkan.h"
#include "Material.h"
#include "MaterialLayout.h"
#include "MaterialLayoutSlot.h"
#include "ShaderBindingTypeVulkan.h"
#include "OSKengine.h"
#include "RendererVulkan.h"
#include "RtShaderTableVulkan.h"
#include "ComputePipelineVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

const DynamicArray<VkCommandBuffer>& CommandListVulkan::GetCommandBuffers() const {
	return commandBuffers;
}

DynamicArray<VkCommandBuffer>* CommandListVulkan::GetCommandBuffers() {
	return &commandBuffers;
}

void CommandListVulkan::Reset() {
	VkResult result = vkResetCommandBuffer(commandBuffers[GetCommandListIndex()], 0);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo resetear la lista de comandos.");
}

void CommandListVulkan::Start() {
	const VkCommandBufferBeginInfo beginInfo {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		NULL,
		0, 
		NULL
	};

	VkResult result = vkBeginCommandBuffer(commandBuffers[GetCommandListIndex()], &beginInfo);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo iniciar la lista de comandos.");
}

void CommandListVulkan::Close() {
	VkResult result = vkEndCommandBuffer(commandBuffers[GetCommandListIndex()]);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo finalizar la lista de comandos.");
}

void CommandListVulkan::TransitionImageLayout(GpuImage* image, GpuImageLayout previous, GpuImageLayout next, TSize baseLayer, TSize numLayers, TSize baseMipLevel, TSize numMipLevels) {
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = GetGpuImageLayoutVulkan(previous);
	barrier.newLayout = GetGpuImageLayoutVulkan(next);
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image->As<GpuImageVulkan>()->GetVkImage();
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = baseMipLevel;
	barrier.subresourceRange.levelCount = numMipLevels == 0 ? VK_REMAINING_MIP_LEVELS : numMipLevels;
	barrier.subresourceRange.baseArrayLayer = baseLayer;
	barrier.subresourceRange.layerCount = numLayers;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;

	VkPipelineStageFlags sourceStage{};
	VkPipelineStageFlags destinationStage{};

	switch (GetGpuImageLayoutVulkan(previous)) {
	case VK_IMAGE_LAYOUT_UNDEFINED:
		barrier.srcAccessMask = 0;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:/*/*/
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		break;

	case VK_IMAGE_LAYOUT_GENERAL:
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;

	default:
		OSK_ASSERT(false, "Layout no soportado.");
		break;
	}

	switch (GetGpuImageLayoutVulkan(next)) {
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		break;

	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		destinationStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		break;

	case VK_IMAGE_LAYOUT_GENERAL:
		destinationStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		break;

	default:
		OSK_ASSERT(false, "Layout no soportado.");
		break;
	}

	vkCmdPipelineBarrier(commandBuffers[GetCommandListIndex()], sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	image->SetLayout(next);
}

void CommandListVulkan::CopyBufferToImage(const GpuDataBuffer* source, GpuImage* dest, TSize layer, TSize offset) {
	VkBufferImageCopy region{};

	region.bufferOffset = source->GetMemorySubblock()->GetOffsetFromBlock() + offset;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = layer;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		dest->GetSize().X,
		dest->GetSize().Y,
		1
	};

	vkCmdCopyBufferToImage(commandBuffers[GetCommandListIndex()],
		source->GetMemoryBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer(), 
		dest->As<GpuImageVulkan>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	// Mip levels
	Vector2i mipSize = { static_cast<int>(dest->GetSize().X), static_cast<int>(dest->GetSize().Y) };
	// El nivel 0 ya está lleno.
	for (TSize mipLevel = 1; mipLevel < dest->GetMipLevels(); mipLevel++) {
		TransitionImageLayout(dest, GpuImageLayout::TRANSFER_DESTINATION, GpuImageLayout::TRANSFER_SOURCE, layer, 1, mipLevel - 1, 1);

		VkImageBlit blit{};

		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipSize.X, mipSize.Y, 1 };

		// Origen: mip level anterior.
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = mipLevel - 1;
		blit.srcSubresource.baseArrayLayer = layer;
		blit.srcSubresource.layerCount = 1;
		
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipSize.X > 1 ? mipSize.X / 2 : 1, mipSize.Y > 1 ? mipSize.Y / 2 : 1, 1 };

		// Destino.
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = mipLevel;
		blit.dstSubresource.baseArrayLayer = layer;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(commandBuffers[GetCommandListIndex()],
			dest->As<GpuImageVulkan>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			dest->As<GpuImageVulkan>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit, VK_FILTER_LINEAR);

		if (mipSize.X > 1) 
			mipSize.X /= 2;
		if (mipSize.Y > 1)
			mipSize.Y /= 2;
	}
	TransitionImageLayout(dest, GpuImageLayout::TRANSFER_DESTINATION, GpuImageLayout::TRANSFER_SOURCE, layer, 1, dest->GetMipLevels() - 1, 1);

	TransitionImageLayout(dest, GpuImageLayout::TRANSFER_SOURCE, GpuImageLayout::TRANSFER_DESTINATION, layer, 1, 0, 0);
}

void CommandListVulkan::CopyImageToImage(const GpuImage* source, GpuImage* destination, TSize numLayers, TSize srcStartLayer, TSize dstStartLayer, TSize srcMipLevel, TSize dstMipLevel, Vector2ui copySize) {
	VkImageCopy region{};

	region.srcOffset = { 0, 0, 0 };
	region.dstOffset = { 0, 0, 0 };

	region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.srcSubresource.mipLevel = srcMipLevel;
	region.srcSubresource.baseArrayLayer = srcStartLayer;
	region.srcSubresource.layerCount = numLayers;

	region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.dstSubresource.mipLevel = dstMipLevel;
	region.dstSubresource.baseArrayLayer = dstStartLayer;
	region.dstSubresource.layerCount = numLayers;

	if (copySize.X == 0)
		copySize.X = destination->GetSize().X;
	if (copySize.Y == 0)
		copySize.Y = destination->GetSize().Y;

	region.extent.width = copySize.X;
	region.extent.height= copySize.Y;
	region.extent.depth = 1; /// @todo Size.Z

	vkCmdCopyImage(commandBuffers[GetCommandListIndex()],
		source->As<GpuImageVulkan>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		destination->As<GpuImageVulkan>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1, &region);
}

void CommandListVulkan::CopyBuffer(const GpuDataBuffer* source, GpuDataBuffer* dest, TSize size, TSize sourceOffset, TSize destOffset) {
	VkBufferCopy copyRegion{};

	copyRegion.srcOffset = sourceOffset + source->GetMemorySubblock()->GetOffsetFromBlock();
	copyRegion.dstOffset = destOffset + dest->GetMemorySubblock()->GetOffsetFromBlock();
	copyRegion.size = size;

	vkCmdCopyBuffer(commandBuffers[GetCommandListIndex()],
		source->GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer(),
		dest->GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer(), 
		1, &copyRegion);
}

void CommandListVulkan::BeginGraphicsRenderpass(DynamicArray<RenderPassImageInfo> colorImages, RenderPassImageInfo depthImage, const Color& color) {
	const Vector2ui targetSize = { colorImages[0].targetImage->GetSize().X, colorImages[0].targetImage->GetSize().Y };

	for (const auto& img : colorImages)
		TransitionImageLayout(img.targetImage, GpuImageLayout::UNDEFINED, GpuImageLayout::COLOR_ATTACHMENT, img.arrayLevel, 1, 0, 0);

	DynamicArray<VkRenderingAttachmentInfo> colorAttachments = DynamicArray<VkRenderingAttachmentInfo>::CreateResizedArray(colorImages.GetSize());
	for (TSize i = 0; i < colorImages.GetSize(); i++) {
		colorAttachments[i] = {};
		colorAttachments[i].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		colorAttachments[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachments[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachments[i].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachments[i].imageView = colorImages[i].targetImage->As<GpuImageVulkan>()->GetColorView(colorImages[i].arrayLevel);
		colorAttachments[i].resolveMode = VK_RESOLVE_MODE_NONE;
		colorAttachments[i].resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachments[i].resolveImageView = VK_NULL_HANDLE;
		colorAttachments[i].clearValue.color = { color.Red, color.Green, color.Blue, color.Alpha };
	}

	OSK_ASSERT(colorAttachments.GetSize() == colorImages.GetSize(), "Error al iniciar el renderpass.");

	VkRenderingAttachmentInfo depthAttachment{};
	depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depthAttachment.imageView = depthImage.targetImage->As<GpuImageVulkan>()->GetDepthStencilView(depthImage.arrayLevel);
	depthAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
	depthAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.resolveImageView = VK_NULL_HANDLE;
	depthAttachment.clearValue.depthStencil = { 1.0f, 0 };

	VkRenderingInfo renderpassInfo{};
	renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderpassInfo.renderArea = { 0, 0, targetSize.X, targetSize.Y };
	renderpassInfo.layerCount = 1;
	renderpassInfo.colorAttachmentCount = colorAttachments.GetSize();
	renderpassInfo.pColorAttachments = colorAttachments.GetData();
	renderpassInfo.pDepthAttachment = &depthAttachment;
	renderpassInfo.pStencilAttachment = &depthAttachment;

	vkCmdBeginRendering(commandBuffers.At(GetCommandListIndex()), &renderpassInfo);

	currentColorImages = colorImages;
	currentDepthImage = depthImage;
}

void CommandListVulkan::EndGraphicsRenderpass() {
	vkCmdEndRendering(commandBuffers[GetCommandListIndex()]);

	const bool isFinal = currentRenderpassType == RenderpassType::FINAL;

	const GpuImageLayout finalLayout = isFinal
		? GpuImageLayout::PRESENT
		: GpuImageLayout::SHADER_READ_ONLY;

	for (const auto& img : currentColorImages)
		TransitionImageLayout(img.targetImage, GpuImageLayout::COLOR_ATTACHMENT, finalLayout, img.arrayLevel, 1, 0, 0);

	if (currentRenderpassType != RenderpassType::INTERMEDIATE)
		currentRenderpassType = RenderpassType::INTERMEDIATE;
}

void CommandListVulkan::BindMaterial(Material* material) {
	currentMaterial = material;

	if (material->IsRaytracing()) {
		currentRtPipeline = material->GetRaytracingPipeline();
		currentPipeline = nullptr;

		vkCmdBindPipeline(commandBuffers[GetCommandListIndex()], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, currentRtPipeline->As<RaytracingPipelineVulkan>()->GetPipeline());
	}
	else {
		DynamicArray<Format> colorFormats;
		for (const auto& colorImg : currentColorImages)
			colorFormats.Insert(colorImg.targetImage->GetFormat());

		currentPipeline = material->GetGraphicsPipeline(colorFormats);
		currentRtPipeline = nullptr;

		vkCmdBindPipeline(commandBuffers[GetCommandListIndex()], VK_PIPELINE_BIND_POINT_GRAPHICS, currentPipeline->As<GraphicsPipelineVulkan>()->GetPipeline());
	}
}

void CommandListVulkan::BindVertexBuffer(const IGpuVertexBuffer* buffer) {
	VkBuffer vertexBuffers[] = { 
		buffer->GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer()
	};
	VkDeviceSize offsets[] = { buffer->GetMemorySubblock()->GetOffsetFromBlock() };

	vkCmdBindVertexBuffers(commandBuffers[GetCommandListIndex()], 0, 1, vertexBuffers, offsets);
}

void CommandListVulkan::BindIndexBuffer(const IGpuIndexBuffer* buffer) {
	vkCmdBindIndexBuffer(commandBuffers[GetCommandListIndex()],
		buffer->GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer(), 
		buffer->GetMemorySubblock()->GetOffsetFromBlock(), VK_INDEX_TYPE_UINT32);
}

void CommandListVulkan::BindMaterialSlot(const IMaterialSlot* slot) {
	VkDescriptorSet sets[] = { slot->As<MaterialSlotVulkan>()->GetDescriptorSet(Engine::GetRenderer()->GetCurrentCommandListIndex()) };

	const VkPipelineBindPoint bindPoint = currentMaterial->IsRaytracing() 
		? VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR 
		: VK_PIPELINE_BIND_POINT_GRAPHICS;

	const VkPipelineLayout layout = currentMaterial->IsRaytracing() 
		? currentRtPipeline->GetLayout()->As<PipelineLayoutVulkan>()->GetLayout()
		: currentPipeline->GetLayout()->As<PipelineLayoutVulkan>()->GetLayout();

	vkCmdBindDescriptorSets(commandBuffers[GetCommandListIndex()], bindPoint, layout,
		currentMaterial->GetLayout()->GetSlot(slot->GetName()).glslSetIndex, 1, sets, 0, nullptr);
}

void CommandListVulkan::PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size, TSize offset) {
	VkPipelineLayout pipelineLayout = currentPipeline->GetLayout()->As<PipelineLayoutVulkan>()->GetLayout();
	auto& pushConstInfo = currentMaterial->GetLayout()->GetPushConstant(pushConstName);

	vkCmdPushConstants(commandBuffers[GetCommandListIndex()], pipelineLayout, GetShaderStageVk(pushConstInfo.stage), pushConstInfo.offset + offset, size, data);
}

void CommandListVulkan::DrawSingleInstance(TSize numIndices) {
	vkCmdDrawIndexed(commandBuffers[GetCommandListIndex()], numIndices, 1, 0, 0, 0);
}

void CommandListVulkan::DrawSingleMesh(TSize firstIndex, TSize numIndices) {
	vkCmdDrawIndexed(commandBuffers[GetCommandListIndex()], numIndices, 1, firstIndex, 0, 0);
}

void CommandListVulkan::TraceRays(TSize raygenEntry, TSize closestHitEntry, TSize missEntry, const Vector2ui& resolution) {
	const RtShaderTableVulkan* shaderTable = currentRtPipeline->GetShaderTable()->As<RtShaderTableVulkan>();

	const VkStridedDeviceAddressRegionKHR raygenTable = shaderTable->GetRaygenTableAddressRegion();
	const VkStridedDeviceAddressRegionKHR closestHitTable = shaderTable->GetClosestHitTableAddressRegion();
	const VkStridedDeviceAddressRegionKHR missTable = shaderTable->GetMissTableAddressRegion();
	const VkStridedDeviceAddressRegionKHR emptyTable{};

	RendererVulkan::pvkCmdTraceRaysKHR(commandBuffers[GetCommandListIndex()],
						&raygenTable, &missTable, &closestHitTable, &emptyTable, 
						resolution.X, resolution.Y, 1);
}

void CommandListVulkan::DispatchCompute(const Vector3ui& groupCount) {
	vkCmdDispatch(commandBuffers[GetCommandListIndex()], groupCount.X, groupCount.Y, groupCount.Z);
}

void CommandListVulkan::BindComputePipeline(const IComputePipeline& computePipeline) {
	vkCmdBindPipeline(commandBuffers[GetCommandListIndex()], VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline.As<ComputePipelineVulkan>()->GetPipeline());
}

void CommandListVulkan::SetViewport(const Viewport& vp) {
	VkViewport viewport{};

	viewport.x = static_cast<float>(vp.rectangle.GetRectanglePosition().X);
	viewport.y = static_cast<float>(vp.rectangle.GetRectanglePosition().Y + vp.rectangle.GetRectangleSize().Y);
	viewport.width = static_cast<float>(vp.rectangle.GetRectangleSize().X);
	viewport.height = -static_cast<float>(vp.rectangle.GetRectangleSize().Y);

	viewport.minDepth = vp.minDepth;
	viewport.maxDepth = vp.maxDepth;

	vkCmdSetViewport(commandBuffers[GetCommandListIndex()], 0, 1, &viewport);
}

void CommandListVulkan::SetScissor(const Vector4ui& scissorRect) {
	VkRect2D scissor{};

	scissor.offset = {
		(int32_t)scissorRect.GetRectanglePosition().X,
		(int32_t)scissorRect.GetRectanglePosition().Y
	};

	scissor.extent = {
		scissorRect.GetRectangleSize().X,
		scissorRect.GetRectangleSize().Y
	};
	
	vkCmdSetScissor(commandBuffers[GetCommandListIndex()], 0, 1, &scissor);
}
