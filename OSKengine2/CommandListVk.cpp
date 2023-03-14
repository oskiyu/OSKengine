#include "CommandListVk.h"

#include <vulkan/vulkan.h>
#include "GpuImageLayoutVk.h"
#include "GpuImageLayout.h"
#include "GpuMemorySubblockVk.h"
#include "GpuMemoryBlockVk.h"
#include "GpuImageVk.h"
#include "Color.hpp"
#include "RenderpassType.h"
#include "IGpuDataBuffer.h"
#include "Viewport.h"
#include "GraphicsPipelineVk.h"
#include "GpuVertexBufferVk.h"
#include "GpuIndexBufferVk.h"
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

using namespace OSK;
using namespace OSK::GRAPHICS;

const DynamicArray<VkCommandBuffer>& CommandListVk::GetCommandBuffers() const {
	return commandBuffers;
}

DynamicArray<VkCommandBuffer>* CommandListVk::GetCommandBuffers() {
	return &commandBuffers;
}

void CommandListVk::Reset() {
	VkResult result = vkResetCommandBuffer(commandBuffers[GetCommandListIndex()], 0);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo resetear la lista de comandos.");
}

void CommandListVk::Start() {
	const VkCommandBufferBeginInfo beginInfo {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		NULL,
		0, // flags
		NULL
	};

	VkResult result = vkBeginCommandBuffer(commandBuffers[GetCommandListIndex()], &beginInfo);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo iniciar la lista de comandos.");
}

void CommandListVk::Close() {
	VkResult result = vkEndCommandBuffer(commandBuffers[GetCommandListIndex()]);
	OSK_ASSERT(result == VK_SUCCESS, "No se pudo finalizar la lista de comandos.");
}

void CommandListVk::SetGpuImageBarrier(GpuImage* image, GpuImageLayout previousLayout, GpuImageLayout nextLayout, GpuBarrierInfo previous, GpuBarrierInfo next, const GpuImageBarrierInfo& prevImageInfo) {
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = GetGpuImageLayoutVk(previousLayout);
	barrier.newLayout = GetGpuImageLayoutVk(nextLayout);
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image->As<GpuImageVk>()->GetVkImage();
	barrier.subresourceRange.aspectMask = 0;
	barrier.subresourceRange.baseMipLevel = prevImageInfo.baseMipLevel;
	barrier.subresourceRange.levelCount = prevImageInfo.numMipLevels;
	barrier.subresourceRange.baseArrayLayer = prevImageInfo.baseLayer;
	barrier.subresourceRange.layerCount = prevImageInfo.numLayers;

	if (EFTraits::HasFlag(prevImageInfo.channel, SampledChannel::COLOR))
		barrier.subresourceRange.aspectMask = barrier.subresourceRange.aspectMask | VK_IMAGE_ASPECT_COLOR_BIT;
	if (EFTraits::HasFlag(prevImageInfo.channel, SampledChannel::DEPTH))
		barrier.subresourceRange.aspectMask = barrier.subresourceRange.aspectMask | VK_IMAGE_ASPECT_DEPTH_BIT;
	if (EFTraits::HasFlag(prevImageInfo.channel, SampledChannel::STENCIL))
		barrier.subresourceRange.aspectMask = barrier.subresourceRange.aspectMask | VK_IMAGE_ASPECT_STENCIL_BIT;

	barrier.srcAccessMask = GetPipelineAccess(previous.accessStage);
	barrier.dstAccessMask = GetPipelineAccess(next.accessStage);

	const VkPipelineStageFlags sourceStage = GetPipelineStage(previous.stage);
	const VkPipelineStageFlags destinationStage = GetPipelineStage(next.stage);

	vkCmdPipelineBarrier(commandBuffers[GetCommandListIndex()], sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void CommandListVk::CopyBufferToImage(const GpuDataBuffer* source, GpuImage* dest, TSize layer, TSize offset) {
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

	// Layout[0] = TRANSFER_DEST
	vkCmdCopyBufferToImage(commandBuffers[GetCommandListIndex()],
		source->GetMemoryBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(), 
		dest->As<GpuImageVk>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	// Mip levels
	Vector2i mipSize = { static_cast<int>(dest->GetSize().X), static_cast<int>(dest->GetSize().Y) };
	// El nivel 0 ya está lleno.
	for (TSize mipLevel = 1; mipLevel < dest->GetMipLevels(); mipLevel++) {

		// Mip anterior: layout DESTINATION -> SOURCE
		SetGpuImageBarrier(dest, GpuImageLayout::TRANSFER_DESTINATION, GpuImageLayout::TRANSFER_SOURCE, 
			GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_READ), 
			{ .baseLayer = layer, .numLayers = 1, .baseMipLevel = mipLevel - 1, .numMipLevels = 1 });

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

		// Mip actual: layout DESTINATION
		vkCmdBlitImage(commandBuffers[GetCommandListIndex()],
			dest->As<GpuImageVk>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			dest->As<GpuImageVk>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit, VK_FILTER_LINEAR);

		if (mipSize.X > 1) 
			mipSize.X /= 2;
		if (mipSize.Y > 1)
			mipSize.Y /= 2;
	}

	// Establecer layout del último nivel = TRANSFER_SOURCE
	SetGpuImageBarrier(dest, GpuImageLayout::TRANSFER_DESTINATION, GpuImageLayout::TRANSFER_SOURCE, 
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_READ),
		{ .baseLayer = layer, .numLayers = 1, .baseMipLevel = dest->GetMipLevels() - 1, .numMipLevels = 1 });

	// Establecer todo TRANSFER_SOURCE -> TRANSFER_DESTINATION
	SetGpuImageBarrier(dest, GpuImageLayout::TRANSFER_SOURCE, GpuImageLayout::TRANSFER_DESTINATION, 
		GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_READ), GpuBarrierInfo(GpuBarrierStage::TRANSFER, GpuBarrierAccessStage::TRANSFER_WRITE),
		{ .baseLayer = layer, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS });
}

void CommandListVk::CopyImageToImage(const GpuImage* source, GpuImage* destination, const CopyImageInfo& copyInfo) {
	VkImageCopy region{};

	region.srcOffset = { 0, 0, 0 };
	region.dstOffset = { 0, 0, 0 };

	const TSize numLayers = copyInfo.numArrayLevels == CopyImageInfo::ALL_ARRAY_LEVELS ? source->GetNumLayers() : copyInfo.numArrayLevels;

	region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.srcSubresource.mipLevel = copyInfo.sourceMipLevel;
	region.srcSubresource.baseArrayLayer = copyInfo.sourceArrayLevel;
	region.srcSubresource.layerCount = numLayers;

	region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.dstSubresource.mipLevel = copyInfo.destinationMipLevel;
	region.dstSubresource.baseArrayLayer = copyInfo.destinationArrayLevel;
	region.dstSubresource.layerCount = numLayers;

	region.extent.width = copyInfo.copySize.X;
	region.extent.height= copyInfo.copySize.Y;
	region.extent.depth = copyInfo.copySize.Z;

	vkCmdCopyImage(commandBuffers[GetCommandListIndex()],
		source->As<GpuImageVk>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		destination->As<GpuImageVk>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1, &region);
}

void CommandListVk::CopyBufferToBuffer(const GpuDataBuffer* source, GpuDataBuffer* dest, TSize size, TSize sourceOffset, TSize destOffset) {
	VkBufferCopy copyRegion{};

	copyRegion.srcOffset = sourceOffset + source->GetMemorySubblock()->GetOffsetFromBlock();
	copyRegion.dstOffset = destOffset + dest->GetMemorySubblock()->GetOffsetFromBlock();
	copyRegion.size = size;

	vkCmdCopyBuffer(commandBuffers[GetCommandListIndex()],
		source->GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(),
		dest->GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(), 
		1, &copyRegion);
}

void CommandListVk::BeginGraphicsRenderpass(DynamicArray<RenderPassImageInfo> colorImages, RenderPassImageInfo depthImage, const Color& color) {
	const Vector2ui targetSize = { colorImages[0].targetImage->GetSize().X, colorImages[0].targetImage->GetSize().Y };

	for (const auto& img : colorImages) {
		SetGpuImageBarrier(img.targetImage, GpuImageLayout::UNDEFINED, GpuImageLayout::COLOR_ATTACHMENT, 
			GpuBarrierInfo(GpuBarrierStage::DEFAULT, GpuBarrierAccessStage::DEFAULT), GpuBarrierInfo(GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT, GpuBarrierAccessStage::COLOR_ATTACHMENT_WRITE), 
			{ .baseLayer = img.arrayLevel, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS, });
	}

	DynamicArray<VkRenderingAttachmentInfo> colorAttachments = DynamicArray<VkRenderingAttachmentInfo>::CreateResizedArray(colorImages.GetSize());
	for (TSize i = 0; i < colorImages.GetSize(); i++) {
		colorAttachments[i] = {};
		colorAttachments[i].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		colorAttachments[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachments[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachments[i].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachments[i].imageView = colorImages[i].targetImage->GetView(SampledChannel::COLOR, SampledArrayType::SINGLE_LAYER, colorImages[i].arrayLevel, 1, ViewUsage::COLOR_TARGET)->As<GpuImageViewVk>()->GetVkView();
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
	depthAttachment.imageView = depthImage.targetImage->GetView(SampledChannel::DEPTH | SampledChannel::STENCIL, SampledArrayType::SINGLE_LAYER, depthImage.arrayLevel, 1, ViewUsage::DEPTH_STENCIL_TARGET)
		->As<GpuImageViewVk>()->GetVkView();
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

	RendererVk::pvkCmdBeginRendering(commandBuffers.At(GetCommandListIndex()), &renderpassInfo);

	currentColorImages = colorImages;
	currentDepthImage = depthImage;
}

void CommandListVk::EndGraphicsRenderpass() {
	RendererVk::pvkCmdEndRendering(commandBuffers[GetCommandListIndex()]);

	const bool isFinal = currentRenderpassType == RenderpassType::FINAL;

	const GpuImageLayout finalLayout = isFinal
		? GpuImageLayout::PRESENT
		: GpuImageLayout::SAMPLED;

	for (const auto& img : currentColorImages) {
		SetGpuImageBarrier(img.targetImage, GpuImageLayout::COLOR_ATTACHMENT, finalLayout,
			GpuBarrierInfo(GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT, GpuBarrierAccessStage::COLOR_ATTACHMENT_WRITE), GpuBarrierInfo(GpuBarrierStage::FRAGMENT_SHADER, GpuBarrierAccessStage::SHADER_READ),
			{ .baseLayer = img.arrayLevel, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS, });
	}

	if (currentRenderpassType != RenderpassType::INTERMEDIATE)
		currentRenderpassType = RenderpassType::INTERMEDIATE;
}

void CommandListVk::BindMaterial(Material* material) {
	currentMaterial = material;

	switch (material->GetMaterialType()) {
	case MaterialType::GRAPHICS: {
		DynamicArray<Format> colorFormats{};
		for (const auto& colorImg : currentColorImages)
			colorFormats.Insert(colorImg.targetImage->GetFormat());

		currentPipeline.graphics = material->GetGraphicsPipeline(colorFormats);

		vkCmdBindPipeline(commandBuffers[GetCommandListIndex()], VK_PIPELINE_BIND_POINT_GRAPHICS, currentPipeline.graphics->As<GraphicsPipelineVk>()->GetPipeline());
	}
		break;

	case MaterialType::RAYTRACING:
		currentPipeline.raytracing = material->GetRaytracingPipeline();
		vkCmdBindPipeline(commandBuffers[GetCommandListIndex()], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, currentPipeline.raytracing->As<RaytracingPipelineVk>()->GetPipeline());

		break;

	case MaterialType::COMPUTE:
		currentPipeline.compute = material->GetComputePipeline();
		vkCmdBindPipeline(commandBuffers[GetCommandListIndex()], VK_PIPELINE_BIND_POINT_COMPUTE, currentPipeline.compute->As<ComputePipelineVk>()->GetPipeline());

		break;
	}
}

void CommandListVk::BindVertexBuffer(const IGpuVertexBuffer* buffer) {
	VkBuffer vertexBuffers[] = { 
		buffer->GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer()
	};
	VkDeviceSize offsets[] = { buffer->GetMemorySubblock()->GetOffsetFromBlock() };

	vkCmdBindVertexBuffers(commandBuffers[GetCommandListIndex()], 0, 1, vertexBuffers, offsets);
}

void CommandListVk::BindIndexBuffer(const IGpuIndexBuffer* buffer) {
	vkCmdBindIndexBuffer(commandBuffers[GetCommandListIndex()],
		buffer->GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVk>()->GetVulkanBuffer(), 
		buffer->GetMemorySubblock()->GetOffsetFromBlock(), VK_INDEX_TYPE_UINT32);
}

void CommandListVk::BindMaterialSlot(const IMaterialSlot* slot) {
	VkDescriptorSet sets[] = { slot->As<MaterialSlotVk>()->GetDescriptorSet(Engine::GetRenderer()->GetCurrentResourceIndex()) };

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

	const TIndex glslSetIndex = currentMaterial->GetLayout()->GetSlot(slot->GetName()).glslSetIndex;
	vkCmdBindDescriptorSets(commandBuffers[GetCommandListIndex()], bindPoint, layout, glslSetIndex, 1, sets, 0, nullptr);
}

void CommandListVk::PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size, TSize offset) {
	VkPipelineLayout layout = VK_NULL_HANDLE;
	switch (currentMaterial->GetMaterialType()) {
		case MaterialType::COMPUTE: layout = currentPipeline.compute->GetLayout()->As<PipelineLayoutVk>()->GetLayout(); break;
		case MaterialType::GRAPHICS: layout = currentPipeline.graphics->GetLayout()->As<PipelineLayoutVk>()->GetLayout(); break;
		case MaterialType::RAYTRACING: layout = currentPipeline.raytracing->GetLayout()->As<PipelineLayoutVk>()->GetLayout(); break;
	}
	auto& pushConstInfo = currentMaterial->GetLayout()->GetPushConstant(pushConstName);

	vkCmdPushConstants(commandBuffers[GetCommandListIndex()], layout, GetShaderStageVk(pushConstInfo.stage), 0 + offset, size, data);
}

void CommandListVk::DrawSingleInstance(TSize numIndices) {
	vkCmdDrawIndexed(commandBuffers[GetCommandListIndex()], numIndices, 1, 0, 0, 0);
}

void CommandListVk::DrawSingleMesh(TSize firstIndex, TSize numIndices) {
	vkCmdDrawIndexed(commandBuffers[GetCommandListIndex()], numIndices, 1, firstIndex, 0, 0);
}

void CommandListVk::TraceRays(TSize raygenEntry, TSize closestHitEntry, TSize missEntry, const Vector2ui& resolution) {
	const RtShaderTableVk* shaderTable = currentPipeline.raytracing->GetShaderTable()->As<RtShaderTableVk>();

	const VkStridedDeviceAddressRegionKHR raygenTable = shaderTable->GetRaygenTableAddressRegion();
	const VkStridedDeviceAddressRegionKHR closestHitTable = shaderTable->GetClosestHitTableAddressRegion();
	const VkStridedDeviceAddressRegionKHR missTable = shaderTable->GetMissTableAddressRegion();
	const VkStridedDeviceAddressRegionKHR emptyTable{};

	RendererVk::pvkCmdTraceRaysKHR(commandBuffers[GetCommandListIndex()],
						&raygenTable, &missTable, &closestHitTable, &emptyTable, 
						resolution.X, resolution.Y, 1);
}

void CommandListVk::DispatchCompute(const Vector3ui& groupCount) {
	vkCmdDispatch(commandBuffers[GetCommandListIndex()], groupCount.X, groupCount.Y, groupCount.Z);
}

void CommandListVk::BindComputePipeline(const IComputePipeline& computePipeline) {
	vkCmdBindPipeline(commandBuffers[GetCommandListIndex()], VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline.As<ComputePipelineVk>()->GetPipeline());
}

void CommandListVk::SetViewport(const Viewport& vp) {
	VkViewport viewport{};

	viewport.x = static_cast<float>(vp.rectangle.GetRectanglePosition().X);
	viewport.y = static_cast<float>(vp.rectangle.GetRectanglePosition().Y + vp.rectangle.GetRectangleSize().Y);
	viewport.width = static_cast<float>(vp.rectangle.GetRectangleSize().X);
	viewport.height = -static_cast<float>(vp.rectangle.GetRectangleSize().Y);

	viewport.minDepth = vp.minDepth;
	viewport.maxDepth = vp.maxDepth;

	vkCmdSetViewport(commandBuffers[GetCommandListIndex()], 0, 1, &viewport);
}

void CommandListVk::SetScissor(const Vector4ui& scissorRect) {
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

void CommandListVk::SetDebugName(const std::string& name) {
	VkDebugUtilsObjectNameInfoEXT nameInfo{};
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType = VK_OBJECT_TYPE_COMMAND_BUFFER;
	nameInfo.pNext = nullptr;

	const VkDevice logicalDevice = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();

	for (TSize i = 0; i < commandBuffers.GetSize(); i++) {
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
	label.color[0] = color.Red;
	label.color[1] = color.Green;
	label.color[2] = color.Blue;
	label.color[3] = color.Alpha;
	label.pNext = nullptr;

	RendererVk::pvkCmdInsertDebugUtilsLabelEXT(commandBuffers[GetCommandListIndex()], &label);
}

void CommandListVk::StartDebugSection(const std::string& mark, const Color& color) {
	VkDebugUtilsLabelEXT label{};
	label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
	label.pLabelName = mark.c_str();
	label.color[0] = color.Red;
	label.color[1] = color.Green;
	label.color[2] = color.Blue;
	label.color[3] = color.Alpha;
	label.pNext = nullptr;

	RendererVk::pvkCmdBeginDebugUtilsLabelEXT(commandBuffers[GetCommandListIndex()], &label);
}

void CommandListVk::EndDebugSection() {
	RendererVk::pvkCmdEndDebugUtilsLabelEXT(commandBuffers[GetCommandListIndex()]);
}


VkPipelineStageFlagBits CommandListVk::GetPipelineStage(GpuBarrierStage stage) const {
	switch (stage) {
	case OSK::GRAPHICS::GpuBarrierStage::VERTEX_SHADER:
		return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
	case OSK::GRAPHICS::GpuBarrierStage::FRAGMENT_SHADER:
		return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	case OSK::GRAPHICS::GpuBarrierStage::TESSELATION_CONTROL:
		return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
	case OSK::GRAPHICS::GpuBarrierStage::TESSELATION_EVALUATION:
		return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
	case OSK::GRAPHICS::GpuBarrierStage::RAYTRACING_SHADER:
		return VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
	case OSK::GRAPHICS::GpuBarrierStage::COLOR_ATTACHMENT_OUTPUT:
		return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	case OSK::GRAPHICS::GpuBarrierStage::TRANSFER:
		return VK_PIPELINE_STAGE_TRANSFER_BIT;
	case OSK::GRAPHICS::GpuBarrierStage::COMPUTE_SHADER:
		return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	case OSK::GRAPHICS::GpuBarrierStage::RT_AS_BUILD:
		return VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
	case OSK::GRAPHICS::GpuBarrierStage::DEPTH_STENCIL_START:
		return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	case OSK::GRAPHICS::GpuBarrierStage::DEPTH_STENCIL_END:
		return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	case OSK::GRAPHICS::GpuBarrierStage::DEFAULT:
		return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	default:
		OSK_ASSERT(false, "No se reconoce el GpuBarrierStage.");
		return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	}
}

VkAccessFlags CommandListVk::GetPipelineAccess(GpuBarrierAccessStage stage) const {
	VkAccessFlags output = 0;

	if (EFTraits::HasFlag(stage, GpuBarrierAccessStage::SHADER_READ))
		output |= VK_ACCESS_SHADER_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuBarrierAccessStage::SHADER_WRITE))
		output |= VK_ACCESS_SHADER_WRITE_BIT;
	if (EFTraits::HasFlag(stage, GpuBarrierAccessStage::COLOR_ATTACHMENT_READ))
		output |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuBarrierAccessStage::COLOR_ATTACHMENT_WRITE))
		output |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	if (EFTraits::HasFlag(stage, GpuBarrierAccessStage::DEPTH_STENCIL_READ))
		output |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuBarrierAccessStage::DEPTH_STENCIL_WRITE))
		output |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	if (EFTraits::HasFlag(stage, GpuBarrierAccessStage::TRANSFER_READ))
		output |= VK_ACCESS_TRANSFER_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuBarrierAccessStage::TRANSFER_WRITE))
		output |= VK_ACCESS_TRANSFER_WRITE_BIT;
	if (EFTraits::HasFlag(stage, GpuBarrierAccessStage::MEMORY_READ))
		output |= VK_ACCESS_MEMORY_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuBarrierAccessStage::MEMORY_WRITE))
		output |= VK_ACCESS_MEMORY_WRITE_BIT;
	if (EFTraits::HasFlag(stage, GpuBarrierAccessStage::RT_AS_READ))
		output |= VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
	if (EFTraits::HasFlag(stage, GpuBarrierAccessStage::RT_AS_WRITE))
		output |= VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
	if (EFTraits::HasFlag(stage, GpuBarrierAccessStage::DEFAULT))
		return 0;

	if (output == 0)
		OSK_ASSERT(false, "No se reconoce el GpuBarrierAccessStage.");

	return output;
}