#include "CommandListVulkan.h"

#include <vulkan/vulkan.h>
#include "GpuImageLayoutVulkan.h"
#include "GpuImageLayout.h"
#include "GpuMemorySubblockVulkan.h"
#include "GpuMemoryBlockVulkan.h"
#include "GpuImageVulkan.h"
#include "RenderpassVulkan.h"
#include "Color.hpp"
#include "RenderpassType.h"
#include "IGpuDataBuffer.h"
#include "Viewport.h"
#include "GraphicsPipelineVulkan.h"
#include "GpuVertexBufferVulkan.h"
#include "GpuIndexBufferVulkan.h"
#include "MaterialSlotVulkan.h"
#include "GraphicsPipelineVulkan.h"
#include "PipelineLayoutVulkan.h"
#include "Material.h"
#include "MaterialLayout.h"
#include "MaterialLayoutSlot.h"
#include "ShaderBindingTypeVulkan.h"

using namespace OSK;

const DynamicArray<VkCommandBuffer>& CommandListVulkan::GetCommandBuffers() const {
	return commandBuffers;
}

DynamicArray<VkCommandBuffer>* CommandListVulkan::GetCommandBuffers() {
	return &commandBuffers;
}

void CommandListVulkan::Reset() {
	for (auto i : commandBuffers) {
		VkResult result = vkResetCommandBuffer(i, 0);
		OSK_ASSERT(result == VK_SUCCESS, "No se pudo resetear la lista de comandos.");
	}
}

void CommandListVulkan::Start() {
	static VkCommandBufferBeginInfo beginInfo {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		NULL,
		0, 
		NULL
	};

	for (auto i : commandBuffers) {
		VkResult result = vkBeginCommandBuffer(i, &beginInfo);
		OSK_ASSERT(result == VK_SUCCESS, "No se pudo iniciar la lista de comandos.");
	}
}

void CommandListVulkan::Close() {
	for (auto i : commandBuffers) {
		VkResult result = vkEndCommandBuffer(i);
		OSK_ASSERT(result == VK_SUCCESS, "No se pudo finalizar la lista de comandos.");
	}
}

void CommandListVulkan::TransitionImageLayout(GpuImage* image, GpuImageLayout previous, GpuImageLayout next) {
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = GetGpuImageLayoutVulkan(previous);
	barrier.newLayout = GetGpuImageLayoutVulkan(next);
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image->As<GpuImageVulkan>()->GetImage();
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	switch (GetGpuImageLayoutVulkan(previous)) {
	case VK_IMAGE_LAYOUT_UNDEFINED:
		barrier.srcAccessMask = 0;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
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
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		break;

	}

	switch (GetGpuImageLayoutVulkan(next)) {
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
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		destinationStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		break;

	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		destinationStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		break;
	}

	for (auto i : commandBuffers)
		vkCmdPipelineBarrier(i, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	image->SetLayout(next);
}

void CommandListVulkan::CopyBufferToImage(const GpuDataBuffer* source, GpuImage* dest) {
	VkBufferImageCopy region{};

	region.bufferOffset = source->GetMemorySubblock()->GetOffsetFromBlock();
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		dest->GetSize().X,
		dest->GetSize().Y,
		1
	};

	vkCmdCopyBufferToImage(commandBuffers[0], 
		source->GetMemoryBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer(), 
		dest->As<GpuImageVulkan>()->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void CommandListVulkan::CopyBuffer(const GpuDataBuffer* source, GpuDataBuffer* dest, TSize size, TSize sourceOffset, TSize destOffset) {
	VkBufferCopy copyRegion{};

	copyRegion.srcOffset = sourceOffset + source->GetMemorySubblock()->GetOffsetFromBlock();
	copyRegion.dstOffset = destOffset + dest->GetMemorySubblock()->GetOffsetFromBlock();
	copyRegion.size = size;

	vkCmdCopyBuffer(commandBuffers[0], 
		source->GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer(),
		dest->GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer(), 
		1, &copyRegion);
}

void CommandListVulkan::BeginRenderpass(IRenderpass* renderpass) {
	BeginAndClearRenderpass(renderpass, Color::BLACK());
}

void CommandListVulkan::BeginAndClearRenderpass(IRenderpass* renderpass, const Color& color) {
	const auto size = renderpass->GetImage(0)->GetSize();

	for (TSize i = 0; i < renderpass->GetNumberOfImages(); i++)
		TransitionImageLayout(renderpass->GetImage(i), GpuImageLayout::UNDEFINED, GpuImageLayout::COLOR_ATTACHMENT);

	for (TSize cmdIndex = 0; cmdIndex < commandBuffers.GetSize(); cmdIndex++) {
		VkRenderPassBeginInfo renderpassInfo{};
		renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderpassInfo.renderPass = renderpass->As<RenderpassVulkan>()->GetRenderpass();
		renderpassInfo.framebuffer = renderpass->As<RenderpassVulkan>()->GetFramebuffer(cmdIndex);
		renderpassInfo.renderArea.offset = { 0,0 };
		renderpassInfo.renderArea.extent = { size.X, size.Y };

		VkClearValue clearColors[3];
		clearColors[0] = { color.Red, color.Green, color.Blue, color.Alpha };
		clearColors[1] = { 1.0f, 0.0f };

		renderpassInfo.clearValueCount = 2;
		renderpassInfo.pClearValues = clearColors;

		vkCmdBeginRenderPass(commandBuffers.At(cmdIndex), &renderpassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	currentRenderpass = renderpass;
}

void CommandListVulkan::EndRenderpass(IRenderpass* renderpass) {
	for (TSize i = 0; i < commandBuffers.GetSize(); i++)
		vkCmdEndRenderPass(commandBuffers.At(i));

	GpuImageLayout finalLayout = GpuImageLayout::SHADER_READ_ONLY;
	if (renderpass->GetType() == RenderpassType::FINAL)
		finalLayout = GpuImageLayout::PRESENT;

	for (TSize i = 0; i < renderpass->GetNumberOfImages(); i++)
		TransitionImageLayout(renderpass->GetImage(i), GpuImageLayout::UNDEFINED, finalLayout);
}

void CommandListVulkan::BindMaterial(const Material* material) {
	currentMaterial = material;
	currentPipeline = material->GetGraphicsPipeline(currentRenderpass);

	for (TSize i = 0; i < commandBuffers.GetSize(); i++)
		vkCmdBindPipeline(commandBuffers.At(i), VK_PIPELINE_BIND_POINT_GRAPHICS, currentPipeline->As<GraphicsPipelineVulkan>()->GetPipeline());
}

void CommandListVulkan::BindVertexBuffer(IGpuVertexBuffer* buffer) {
	VkBuffer vertexBuffers[] = { 
		buffer->GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer() 
	};
	VkDeviceSize offsets[] = { buffer->GetMemorySubblock()->GetOffsetFromBlock() };

	for (TSize i = 0; i < commandBuffers.GetSize(); i++)
		vkCmdBindVertexBuffers(commandBuffers.At(i), 0, 1, vertexBuffers, offsets);
}

void CommandListVulkan::BindIndexBuffer(IGpuIndexBuffer* buffer) {
	for (TSize i = 0; i < commandBuffers.GetSize(); i++)
		vkCmdBindIndexBuffer(commandBuffers.At(i),
			buffer->GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVulkan>()->GetVulkanBuffer(), 
			buffer->GetMemorySubblock()->GetOffsetFromBlock(), VK_INDEX_TYPE_UINT32);
}

void CommandListVulkan::BindMaterialSlot(const IMaterialSlot* slot) {
	for (TSize i = 0; i < commandBuffers.GetSize(); i++) {
		VkDescriptorSet sets[] = { slot->As<MaterialSlotVulkan>()->GetDescriptorSet(i) };

		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, currentPipeline->GetLayout()->As<PipelineLayoutVulkan>()->GetLayout(),
			currentMaterial->GetLayout()->GetSlot(slot->GetName()).glslSetIndex, 1, sets, 0, nullptr);
	}
}

void CommandListVulkan::PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size, TSize offset) {
	VkPipelineLayout pipelineLayout = currentPipeline->GetLayout()->As<PipelineLayoutVulkan>()->GetLayout();
	auto& pushConstInfo = currentMaterial->GetLayout()->GetPushConstant(pushConstName);

	for (TSize i = 0; i < commandBuffers.GetSize(); i++)
		vkCmdPushConstants(commandBuffers[i], pipelineLayout, GetShaderStageVk(pushConstInfo.stage), pushConstInfo.offset + offset, size, data);
}

void CommandListVulkan::DrawSingleInstance(TSize numIndices) {
	for (TSize i = 0; i < commandBuffers.GetSize(); i++)
		vkCmdDrawIndexed(commandBuffers[i], numIndices, 1, 0, 0, 0);
}

void CommandListVulkan::SetViewport(const Viewport& vp) {
	VkViewport viewport{};

	viewport.x = vp.rectangle.GetRectanglePosition().X;
	viewport.y = vp.rectangle.GetRectanglePosition().Y + vp.rectangle.GetRectangleSize().Y;
	viewport.width = vp.rectangle.GetRectangleSize().X;
	viewport.height = -((int)vp.rectangle.GetRectangleSize().Y);

	viewport.minDepth = vp.minDepth;
	viewport.maxDepth = vp.maxDepth;

	for (TSize i = 0; i < commandBuffers.GetSize(); i++)
		vkCmdSetViewport(commandBuffers.At(i), 0, 1, &viewport);
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
	
	for (TSize i = 0; i < commandBuffers.GetSize(); i++)
		vkCmdSetScissor(commandBuffers.At(i), 0, 1, &scissor);
}
