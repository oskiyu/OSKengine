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
}

void CommandListVulkan::EndRenderpass(IRenderpass* renderpass) {
	for (TSize i = 0; i < commandBuffers.GetSize(); i++)
		vkCmdEndRenderPass(commandBuffers.At(i));

	GpuImageLayout finalLayout = GpuImageLayout::SHADER_READ_ONLY;
	if (renderpass->GetType() == RenderpassType::FINAL)
		finalLayout = GpuImageLayout::PRESENT;

	//for (TSize i = 0; i < renderpass->GetNumberOfImages(); i++)
		//TransitionImageLayout(renderpass->GetImage(i), GpuImageLayout::COLOR_ATTACHMENT, finalLayout);
}
