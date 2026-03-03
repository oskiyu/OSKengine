#include "CommandListVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include <vulkan/vulkan.h>

// Memoria GPU
#include "GpuMemorySubblockVk.h"
#include "GpuMemoryBlockVk.h"
#include "GpuBuffer.h"

// Imágenes GPU
#include "GpuImageLayoutVk.h"
#include "GpuImageLayout.h"
#include "GpuImageVk.h"
#include "GpuImageViewVk.h"
#include "GpuImageSamplerVk.h"

#include "CopyImageInfo.h"

// Materials
#include "Material.h"
#include "MaterialLayout.h"
#include "MaterialLayoutSlot.h"

#include "MaterialSlotVk.h"

// Pipelines
#include "GraphicsPipelineVk.h"
#include "RaytracingPipelineVk.h"
#include "ComputePipelineVk.h"
#include "MeshPipelineVk.h"

#include "PipelineLayoutVk.h"

// RT
#include "ShaderBindingTypeVk.h"
#include "RtShaderTableVk.h"

// Buffer
#include "GpuBufferRange.h"

// Otros
#include "Color.hpp"
#include "RenderpassType.h"
#include "Viewport.h"

// Para funciones de comandos de extensiones.
#include "RendererVk.h"
// Para obtener el índice de frame de recursos.
#include "OSKengine.h"

// Excepciones
#include "CommandListExceptions.h"
#include "NotImplementedException.h"

// Para creación
#include "CommandPoolVk.h"
#include "GpuVk.h"

// Transferencia de colas
#include "CommandQueueVk.h"
#include "UnreachableException.h"


using namespace OSK;
using namespace OSK::GRAPHICS;


static VkPipelineStageFlagBits2 GetPipelineStageVk(GpuCommandStage stage) {
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

static VkPipelineStageFlagBits GetPipelineStageVk_1_0(GpuCommandStage stage) {
	switch (stage) {
	case GpuCommandStage::VERTEX_SHADER:
		return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
	case GpuCommandStage::FRAGMENT_SHADER:
		return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

	case GpuCommandStage::TESSELATION_CONTROL:
		return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
	case GpuCommandStage::TESSELATION_EVALUATION:
		return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;

	case GpuCommandStage::RAYTRACING_SHADER:
		return VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
	case GpuCommandStage::RT_AS_BUILD:
		return VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

	case GpuCommandStage::COLOR_ATTACHMENT_OUTPUT:
		return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	case GpuCommandStage::DEPTH_STENCIL_START:
		return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	case GpuCommandStage::DEPTH_STENCIL_END:
		return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

	case GpuCommandStage::TRANSFER:
		return VK_PIPELINE_STAGE_TRANSFER_BIT;

	case GpuCommandStage::COMPUTE_SHADER:
		return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

	case GpuCommandStage::NONE:
		return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	case GpuCommandStage::ALL:
		return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

	default:
		OSK_ASSERT(false, NotImplementedException());
		return VK_PIPELINE_STAGE_NONE;
	}
}

static VkAccessFlags2 GetPipelineAccessVk(GpuAccessStage stage) {
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

static VkAccessFlags GetPipelineAccessVk_1_0(GpuAccessStage stage) {
	VkAccessFlags output = 0;

	if (EFTraits::HasFlag(stage, GpuAccessStage::SHADER_READ))
		output |= VK_ACCESS_SHADER_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuAccessStage::SHADER_WRITE))
		output |= VK_ACCESS_SHADER_WRITE_BIT;

	if (EFTraits::HasFlag(stage, GpuAccessStage::SAMPLED_READ))
		output |= VK_ACCESS_SHADER_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuAccessStage::STORAGE_BUFFER_READ))
		output |= VK_ACCESS_SHADER_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuAccessStage::STORAGE_IMAGE_READ))
		output |= VK_ACCESS_SHADER_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuAccessStage::UNIFORM_BUFFER_READ))
		output |= VK_ACCESS_UNIFORM_READ_BIT;

	if (EFTraits::HasFlag(stage, GpuAccessStage::COLOR_ATTACHMENT_READ))
		output |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuAccessStage::COLOR_ATTACHMENT_WRITE))
		output |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	if (EFTraits::HasFlag(stage, GpuAccessStage::DEPTH_STENCIL_READ))
		output |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuAccessStage::DEPTH_STENCIL_WRITE))
		output |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	if (EFTraits::HasFlag(stage, GpuAccessStage::TRANSFER_READ))
		output |= VK_ACCESS_TRANSFER_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuAccessStage::TRANSFER_WRITE))
		output |= VK_ACCESS_TRANSFER_WRITE_BIT;

	if (EFTraits::HasFlag(stage, GpuAccessStage::MEMORY_READ))
		output |= VK_ACCESS_MEMORY_READ_BIT;
	if (EFTraits::HasFlag(stage, GpuAccessStage::MEMORY_WRITE))
		output |= VK_ACCESS_MEMORY_WRITE_BIT;

	if (EFTraits::HasFlag(stage, GpuAccessStage::RT_AS_READ))
		output |= VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
	if (EFTraits::HasFlag(stage, GpuAccessStage::RT_AS_WRITE))
		output |= VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;

	if (EFTraits::HasFlag(stage, GpuAccessStage::NONE))
		output |= VK_ACCESS_NONE;

	return output;
}

static VkIndexType GetIndexTypevk(IndexType type) {
	switch (type)
	{
	case OSK::GRAPHICS::IndexType::U8:
		return VK_INDEX_TYPE_UINT8_EXT;
	case OSK::GRAPHICS::IndexType::U16:
		return VK_INDEX_TYPE_UINT16;
	case OSK::GRAPHICS::IndexType::U32:
		return VK_INDEX_TYPE_UINT32;

	default:
		throw UnreachableException("IndexType no reconocido.");
	}
}


template <VulkanTarget Target>
CommandListVk<Target>::CommandListVk(const GpuVk<Target>& gpu, CommandPoolVk<Target>* commandPool) : ICommandList(commandPool), m_logicalDevice(gpu.GetLogicalDevice()) {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool->GetCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<USize32>(m_commandBuffers.size());

	VkResult result = vkAllocateCommandBuffers(
		m_logicalDevice, 
		&allocInfo, 
		m_commandBuffers.data());

	OSK_ASSERT(result == VK_SUCCESS, CommandListCreationException(result));
}


template <VulkanTarget Target>
std::span<const VkCommandBuffer> CommandListVk<Target>::GetCommandBuffers() const {
	return m_commandBuffers;
}

template <VulkanTarget Target>
void CommandListVk<Target>::Reset() {
	VkResult result = vkResetCommandBuffer(m_commandBuffers[_GetCommandListIndex()], 0);
	OSK_ASSERT(result == VK_SUCCESS, CommandListResetException(result));
}

template <VulkanTarget Target>
void CommandListVk<Target>::Start() {
	const VkCommandBufferBeginInfo beginInfo {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		0, // flags
		nullptr
	};

	VkResult result = vkBeginCommandBuffer(m_commandBuffers[_GetCommandListIndex()], &beginInfo);
	OSK_ASSERT(result == VK_SUCCESS, CommandListStartException(result));
}

template <VulkanTarget Target>
void CommandListVk<Target>::Close() {
	VkResult result = vkEndCommandBuffer(m_commandBuffers[_GetCommandListIndex()]);
	OSK_ASSERT(result == VK_SUCCESS, CommandListEndException(result));
}

template <VulkanTarget Target>
void CommandListVk<Target>::SetGpuImageBarrier(
	GpuImage* image, 
	GpuImageLayout previousLayout, 
	GpuImageLayout nextLayout, 
	GpuBarrierInfo previous, 
	GpuBarrierInfo next, 
	const GpuImageRange& imageInfo, 
	const ResourceQueueTransferInfo queueTranfer) 
{
	// Usar barrera normal para Vulkan 1.0.
	if constexpr (Target == VulkanTarget::VK_1_0) {
		VkImageMemoryBarrier barrier{};

		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

		barrier.oldLayout = GetGpuImageLayoutVk(previousLayout);
		barrier.newLayout = GetGpuImageLayoutVk(nextLayout);

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image->As<GpuImageVk<Target>>()->GetVkImage();

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

		barrier.srcAccessMask = GetPipelineAccessVk_1_0(previous.accessStage);
		barrier.dstAccessMask = GetPipelineAccessVk_1_0(next.accessStage);

		const auto srcStage = GetPipelineStageVk_1_0(previous.stage);
		const auto dstStage = GetPipelineStageVk_1_0(next.stage);

		vkCmdPipelineBarrier(
			m_commandBuffers[_GetCommandListIndex()],
			srcStage,
			dstStage,
			0, // flags
			0, // memory barrier count
			nullptr, // memory barriers
			0, // buffer barrier count,
			nullptr, // buffer barriers,
			1,
			&barrier);
	}

	if constexpr (Target == VulkanTarget::VK_LATEST) {
		VkImageMemoryBarrier2 barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;

		barrier.oldLayout = GetGpuImageLayoutVk(previousLayout);
		barrier.newLayout = GetGpuImageLayoutVk(nextLayout);

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image->As<GpuImageVk<Target>>()->GetVkImage();

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

		barrier.srcAccessMask = GetPipelineAccessVk(previous.accessStage);
		barrier.dstAccessMask = GetPipelineAccessVk(next.accessStage);

		barrier.srcStageMask = GetPipelineStageVk(previous.stage);
		barrier.dstStageMask = GetPipelineStageVk(next.stage);

		// Queue transfer.
		if (queueTranfer.transfer) {
			barrier.srcQueueFamilyIndex = queueTranfer.sourceQueue->As<CommandQueueVk<Target>>()->GetFamily().familyIndex;
			barrier.dstQueueFamilyIndex = queueTranfer.destinationQueue->As<CommandQueueVk<Target>>()->GetFamily().familyIndex;
		}

		VkDependencyInfo info{};
		info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;

		info.imageMemoryBarrierCount = 1;
		info.pImageMemoryBarriers = &barrier;
		info.dependencyFlags = 0;

		vkCmdPipelineBarrier2(m_commandBuffers[_GetCommandListIndex()], &info);
	}

	// Código común.

	image->SetCurrentBarrier(next);
	image->_SetLayout(
		imageInfo.baseLayer,
		imageInfo.numLayers,
		imageInfo.baseMipLevel,
		imageInfo.numMipLevels,
		nextLayout);

	if (queueTranfer.transfer) {
		// TODO: image->_UpdateOwnerQueue()
	}
}

template <VulkanTarget Target>
void CommandListVk<Target>::SetGpuBufferBarrier(
	GpuBuffer* buffer, 
	const GpuBufferRange& range,
	GpuBarrierInfo previous, 
	GpuBarrierInfo next,
	const ResourceQueueTransferInfo queueTranfer) 
{
	if constexpr (Target == VulkanTarget::VK_1_0) {
		VkBufferMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;

		barrier.buffer = buffer->GetMemoryBlock()->As<GpuMemoryBlockVk<Target>>()->GetVulkanBuffer();

		barrier.offset = range.offset;
		barrier.size = range.size;

		barrier.srcAccessMask = GetPipelineAccessVk_1_0(previous.accessStage);
		barrier.dstAccessMask = GetPipelineAccessVk_1_0(next.accessStage);

		const auto srcStage = GetPipelineStageVk_1_0(previous.stage);
		const auto dstStage = GetPipelineStageVk_1_0(next.stage);

		// Queue transfer.
		if (queueTranfer.transfer) {
			barrier.srcQueueFamilyIndex = queueTranfer.sourceQueue->As<CommandQueueVk<Target>>()->GetFamily().familyIndex;
			barrier.dstQueueFamilyIndex = queueTranfer.destinationQueue->As<CommandQueueVk<Target>>()->GetFamily().familyIndex;
		}

		barrier.pNext = nullptr;

		vkCmdPipelineBarrier(
			m_commandBuffers[_GetCommandListIndex()],
			srcStage,
			dstStage,
			0, // flags
			0, // memory barrier count
			nullptr, // memory barriers
			1, // buffer barrier count,
			&barrier, // buffer barriers,
			0,
			nullptr);
	}

	if constexpr (Target == VulkanTarget::VK_LATEST) {
		VkBufferMemoryBarrier2 barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;

		barrier.buffer = buffer->GetMemoryBlock()->As<GpuMemoryBlockVk<Target>>()->GetVulkanBuffer();

		barrier.offset = range.offset;
		barrier.size = range.size;

		barrier.srcAccessMask = GetPipelineAccessVk(previous.accessStage);
		barrier.dstAccessMask = GetPipelineAccessVk(next.accessStage);

		barrier.srcStageMask = GetPipelineStageVk(previous.stage);
		barrier.dstStageMask = GetPipelineStageVk(next.stage);

		// Queue transfer.
		if (queueTranfer.transfer) {
			barrier.srcQueueFamilyIndex = queueTranfer.sourceQueue->As<CommandQueueVk<Target>>()->GetFamily().familyIndex;
			barrier.dstQueueFamilyIndex = queueTranfer.destinationQueue->As<CommandQueueVk<Target>>()->GetFamily().familyIndex;
		}

		barrier.pNext = nullptr;

		// Submit.
		VkDependencyInfo info{};
		info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;

		info.bufferMemoryBarrierCount = 1;
		info.pBufferMemoryBarriers = &barrier;
		info.dependencyFlags = 0;

		vkCmdPipelineBarrier2(m_commandBuffers[_GetCommandListIndex()], &info);
	}

	// Código común.
	
	// Update state
	buffer->_UpdateCurrentBarrier(next);

	if (queueTranfer.transfer) {
		// TODO: buffer->_UpdateOwnerQueue(Engine::GetRenderer()->GetOptimalQueue();
	}
}

template <VulkanTarget Target>
void CommandListVk<Target>::CopyBufferToImage(const GpuBuffer& source, GpuImage* dest, UIndex32 layer, USize64 offset) {
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

	// `Layout[0] = TRANSFER_DEST`
	vkCmdCopyBufferToImage(
		m_commandBuffers[_GetCommandListIndex()],
		source.GetMemoryBlock()->As<GpuMemoryBlockVk<Target>>()->GetVulkanBuffer(),
		dest->As<GpuImageVk<Target>>()->GetVkImage(),
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
				{ .baseLayer = layer, .numLayers = 1, .baseMipLevel = mipLevel - 1, .numMipLevels = 1 },
				ResourceQueueTransferInfo::Empty());

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
			vkCmdBlitImage(m_commandBuffers[_GetCommandListIndex()],
				dest->As<GpuImageVk<Target>>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				dest->As<GpuImageVk<Target>>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
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
			{ .baseLayer = layer, .numLayers = 1, .baseMipLevel = dest->GetMipLevels() - 1, .numMipLevels = 1 },
			ResourceQueueTransferInfo::Empty());

		// Establecer todo TRANSFER_SOURCE -> TRANSFER_DESTINATION
		SetGpuImageBarrier(
			dest,
			GpuImageLayout::TRANSFER_SOURCE,
			GpuImageLayout::TRANSFER_DESTINATION,
			GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_READ),
			GpuBarrierInfo(GpuCommandStage::TRANSFER, GpuAccessStage::TRANSFER_WRITE),
			{ .baseLayer = layer, .numLayers = 1, .baseMipLevel = 0, .numMipLevels = ALL_MIP_LEVELS },
			ResourceQueueTransferInfo::Empty());
	}
}

template <VulkanTarget Target>
void CommandListVk<Target>::RawCopyImageToImage(const GpuImage& source, GpuImage* destination, const CopyImageInfo& copyInfo) {
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

	region.srcSubresource.aspectMask = GpuImageVk<Target>::GetAspectFlags(copyInfo.sourceChannel);
	region.srcSubresource.mipLevel = copyInfo.sourceMipLevel;
	region.srcSubresource.baseArrayLayer = copyInfo.sourceArrayLevel;
	region.srcSubresource.layerCount = numLayers;

	region.dstSubresource.aspectMask = GpuImageVk<Target>::GetAspectFlags(copyInfo.destinationChannel);
	region.dstSubresource.mipLevel = copyInfo.destinationMipLevel;
	region.dstSubresource.baseArrayLayer = copyInfo.destinationArrayLevel;
	region.dstSubresource.layerCount = numLayers;

	region.extent.width = copyInfo.copySize.x;
	region.extent.height= copyInfo.copySize.y;
	region.extent.depth = copyInfo.copySize.z;

	vkCmdCopyImage(m_commandBuffers[_GetCommandListIndex()],
		source.As<GpuImageVk<Target>>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		destination->As<GpuImageVk<Target>>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1, &region);
}

template <VulkanTarget Target>
void CommandListVk<Target>::CopyImageToImage(const GpuImage& source, GpuImage* destination, const CopyImageInfo& copyInfo, GpuImageFilteringType filter) {
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
	sourceLayer.aspectMask = GpuImageVk<Target>::GetAspectFlags(copyInfo.sourceChannel);
	sourceLayer.baseArrayLayer = copyInfo.sourceArrayLevel;
	sourceLayer.layerCount = numLayers;
	sourceLayer.mipLevel = copyInfo.sourceMipLevel;

	copyRegion.srcSubresource = sourceLayer;

	VkImageSubresourceLayers destinationLayer{};
	destinationLayer.aspectMask = GpuImageVk<Target>::GetAspectFlags(copyInfo.destinationChannel);
	destinationLayer.baseArrayLayer = copyInfo.destinationArrayLevel;
	destinationLayer.layerCount = numLayers;
	destinationLayer.mipLevel = copyInfo.destinationMipLevel;

	copyRegion.dstSubresource = destinationLayer;

	vkCmdBlitImage(
		m_commandBuffers[_GetCommandListIndex()],
		source.As<GpuImageVk<Target>>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		destination->As<GpuImageVk<Target>>()->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1, &copyRegion,
		GpuImageSamplerVk<Target>::GetFilterTypeVk(filter));
}

template <VulkanTarget Target>
void CommandListVk<Target>::CopyBufferToBuffer(const GpuBuffer& source, GpuBuffer* dest, USize64 size, USize64 sourceOffset, USize64 destOffset) {
	VkBufferCopy copyRegion{};

	copyRegion.srcOffset = sourceOffset + source.GetMemorySubblock()->GetOffsetFromBlock();
	copyRegion.dstOffset = destOffset + dest->GetMemorySubblock()->GetOffsetFromBlock();
	copyRegion.size = size;

	vkCmdCopyBuffer(
		m_commandBuffers[_GetCommandListIndex()],
		source.GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVk<Target>>()->GetVulkanBuffer(),
		dest->GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVk<Target>>()->GetVulkanBuffer(), 
		1, &copyRegion);
}

template <VulkanTarget Target>
void CommandListVk<Target>::ClearImage(GpuImage* image, const GpuImageRange& range, const Color& color) {
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
		m_commandBuffers[_GetCommandListIndex()],
		image->As<GpuImageVk<Target>>()->GetVkImage(), 
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
		&vkColor, 1, &vkRange);
}

template <VulkanTarget Target>
void CommandListVk<Target>::BeginGraphicsRenderpass(
	DynamicArray<RenderPassImageInfo> colorImages, 
	RenderPassImageInfo depthImage, 
	const Color& color, 
	bool autoSync) 
{
	const Vector2ui targetSize = colorImages[0].targetImage->GetSize2D();

	if (autoSync) {
		for (const auto& img : colorImages) {
			if (img.clear) {
				SetGpuImageBarrier(
					img.targetImage,
					GpuImageLayout::UNDEFINED,
					GpuImageLayout::COLOR_ATTACHMENT,
					GpuBarrierInfo(GpuCommandStage::NONE, GpuAccessStage::NONE),
					GpuBarrierInfo(GpuCommandStage::COLOR_ATTACHMENT_OUTPUT, GpuAccessStage::COLOR_ATTACHMENT_WRITE),
					{ .baseLayer = img.arrayLevel,
						.numLayers = 1,
						.baseMipLevel = 0,
						.numMipLevels = ALL_MIP_LEVELS, },
					ResourceQueueTransferInfo::Empty());
			}
			else {
				const GpuImageLayout previousLayout = img.targetImage->_GetLayout(img.arrayLevel, 0);
				SetGpuImageBarrier(
					img.targetImage,
					previousLayout,
					GpuImageLayout::COLOR_ATTACHMENT,
					img.targetImage->GetCurrentBarrier(),
					GpuBarrierInfo(GpuCommandStage::COLOR_ATTACHMENT_OUTPUT, GpuAccessStage::COLOR_ATTACHMENT_WRITE),
					GpuImageRange{ .baseLayer = img.arrayLevel,
						.numLayers = 1,
						.baseMipLevel = 0,
						.numMipLevels = ALL_MIP_LEVELS, },
					ResourceQueueTransferInfo::Empty());
			}
		}

		SampledChannel depthChannel = SampledChannel::DEPTH;
		if (FormatSupportsStencil(depthImage.targetImage->GetFormat()))
			depthChannel |= SampledChannel::STENCIL;

		if (depthImage.clear) {
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
					.channel = depthChannel },
				ResourceQueueTransferInfo::Empty());
		}
		else {
			const GpuImageLayout previousLayout = depthImage.targetImage->_GetLayout(depthImage.arrayLevel, 0);
			SetGpuImageBarrier(
				depthImage.targetImage,
				previousLayout,
				GpuImageLayout::DEPTH_STENCIL_TARGET,
				depthImage.targetImage->GetCurrentBarrier(),
				GpuBarrierInfo(GpuCommandStage::DEPTH_STENCIL_START, GpuAccessStage::DEPTH_STENCIL_READ | GpuAccessStage::DEPTH_STENCIL_WRITE),
				{ .baseLayer = depthImage.arrayLevel,
					.numLayers = 1,
					.baseMipLevel = 0,
					.numMipLevels = ALL_MIP_LEVELS,
					.channel = depthChannel },
				ResourceQueueTransferInfo::Empty());
		}
	}

	GpuImageViewConfig colorAttachmentConfig = GpuImageViewConfig::CreateTarget_Color();
	DynamicArray<VkRenderingAttachmentInfo> colorAttachments = DynamicArray<VkRenderingAttachmentInfo>::CreateResized(colorImages.GetSize());
	for (UIndex64 i = 0; i < colorImages.GetSize(); i++) {
		colorAttachmentConfig.baseArrayLevel = colorImages[i].arrayLevel;

		colorAttachments[i] = {};
		colorAttachments[i].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		colorAttachments[i].loadOp = colorImages[i].clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachments[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachments[i].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachments[i].imageView = colorImages[i].targetImage->GetView(colorAttachmentConfig)->As<GpuImageViewVk<Target>>()->GetVkView();
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
	depthAttachment.loadOp = depthImage.clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.imageLayout = FormatSupportsStencil(depthImage.targetImage->GetFormat()) 
		? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		: VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
	depthAttachment.imageView = depthImage.targetImage->GetView(dephtAttachmentConfig)->As<GpuImageViewVk<Target>>()->GetVkView();
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

	RendererVk<Target>::pvkCmdBeginRendering(m_commandBuffers[_GetCommandListIndex()], &renderpassInfo);

	m_currentlyBoundColorImages = colorImages;
	m_currentlyBoundDepthImage = depthImage;
}

template <VulkanTarget Target>
void CommandListVk<Target>::EndGraphicsRenderpass(bool autoSync) {
	RendererVk<Target>::pvkCmdEndRendering(m_commandBuffers[_GetCommandListIndex()]);

	const bool isFinal = m_currentlyBoundRenderTargetType == RenderpassType::FINAL;

	if (m_currentlyBoundRenderTargetType != RenderpassType::INTERMEDIATE) {
		m_currentlyBoundRenderTargetType = RenderpassType::INTERMEDIATE;
	}
}

template <VulkanTarget Target>
void CommandListVk<Target>::BindVertexBufferRange(const GpuBuffer& buffer, const VertexBufferView& view) {
	VkBuffer vertexBuffer = buffer.GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVk<Target>>()->GetVulkanBuffer();
	VkDeviceSize offset = buffer.GetMemorySubblock()->GetOffsetFromBlock() + view.offsetInBytes;

	vkCmdBindVertexBuffers(m_commandBuffers[_GetCommandListIndex()], 0, 1, &vertexBuffer, &offset);
}

template <VulkanTarget Target>
void CommandListVk<Target>::BindIndexBufferRange(const GpuBuffer& buffer, const IndexBufferView& view) {
	vkCmdBindIndexBuffer(
		m_commandBuffers[_GetCommandListIndex()],
		buffer.GetMemorySubblock()->GetOwnerBlock()->As<GpuMemoryBlockVk<Target>>()->GetVulkanBuffer(),
		buffer.GetMemorySubblock()->GetOffsetFromBlock() + view.offsetInBytes, 
		GetIndexTypevk(view.type));
}

template <VulkanTarget Target>
void CommandListVk<Target>::BindMaterialSlot(const IMaterialSlot& slot) {
	VkDescriptorSet set = slot.As<MaterialSlotVk<Target>>()->GetDescriptorSet();

	VkPipelineBindPoint bindPoint{};
	switch (m_currentlyBoundMaterial->GetMaterialType()) {
		case MaterialType::COMPUTE:		bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;			break;
		case MaterialType::GRAPHICS:	bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;		break;
		case MaterialType::RAYTRACING:	bindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR; break;
		case MaterialType::MESH:		bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;		break;
	}

	VkPipelineLayout layout = VK_NULL_HANDLE;
	switch (m_currentlyBoundMaterial->GetMaterialType()) {
		case MaterialType::COMPUTE: layout = m_currentPipeline.compute->GetLayout()->As<PipelineLayoutVk<Target>>()->GetLayout(); break;
		case MaterialType::GRAPHICS: layout = m_currentPipeline.graphics->GetLayout()->As<PipelineLayoutVk<Target>>()->GetLayout(); break;
		case MaterialType::RAYTRACING: layout = m_currentPipeline.raytracing->GetLayout()->As<PipelineLayoutVk<Target>>()->GetLayout(); break;
		case MaterialType::MESH: layout = m_currentPipeline.mesh->GetLayout()->As<PipelineLayoutVk<Target>>()->GetLayout(); break;
	}

	const auto glslSetIndex = m_currentlyBoundMaterial->GetLayout()->GetSlot(slot.GetName()).glslSetIndex;
	vkCmdBindDescriptorSets(m_commandBuffers[_GetCommandListIndex()], bindPoint, layout, glslSetIndex, 1, std::addressof(set), 0, nullptr);
}

template <VulkanTarget Target>
void CommandListVk<Target>::PushMaterialConstants(const std::string& pushConstName, const void* data, USize32 size, USize32 offset) {
	VkPipelineLayout layout = VK_NULL_HANDLE;
	switch (m_currentlyBoundMaterial->GetMaterialType()) {
		case MaterialType::COMPUTE: layout = m_currentPipeline.compute->GetLayout()->As<PipelineLayoutVk<Target>>()->GetLayout(); break;
		case MaterialType::GRAPHICS: layout = m_currentPipeline.graphics->GetLayout()->As<PipelineLayoutVk<Target>>()->GetLayout(); break;
		case MaterialType::RAYTRACING: layout = m_currentPipeline.raytracing->GetLayout()->As<PipelineLayoutVk<Target>>()->GetLayout(); break;
		case MaterialType::MESH: layout = m_currentPipeline.mesh->GetLayout()->As<PipelineLayoutVk<Target>>()->GetLayout(); break;
	}
	auto& pushConstInfo = m_currentlyBoundMaterial->GetLayout()->GetPushConstant(pushConstName);

	vkCmdPushConstants(m_commandBuffers[_GetCommandListIndex()], layout, GetShaderStageVk(pushConstInfo.stage), 0 + offset, size, data);
}

template <VulkanTarget Target>
void CommandListVk<Target>::DrawSingleInstance(USize32 numIndices) {
	vkCmdDrawIndexed(m_commandBuffers[_GetCommandListIndex()], numIndices, 1, 0, 0, 0);
}

template <VulkanTarget Target>
void CommandListVk<Target>::DrawSingleMesh(UIndex32 firstIndex, USize32 numIndices) {
	vkCmdDrawIndexed(m_commandBuffers[_GetCommandListIndex()], numIndices, 1, firstIndex, 0, 0);
}

template <VulkanTarget Target>
void CommandListVk<Target>::DrawInstances(UIndex32 firstIndex, USize32 numIndices, UIndex32 firstInstance, USize32 instanceCount) {
	vkCmdDrawIndexed(m_commandBuffers[_GetCommandListIndex()], numIndices, instanceCount, firstIndex, 0, firstInstance);
}

template <VulkanTarget Target>
void CommandListVk<Target>::TraceRays(UIndex32 raygenEntry, UIndex32 closestHitEntry, UIndex32 missEntry, const Vector2ui& resolution) {
	const RtShaderTableVk* shaderTable = m_currentPipeline.raytracing->GetShaderTable()->As<RtShaderTableVk>();

	const VkStridedDeviceAddressRegionKHR raygenTable = shaderTable->GetRaygenTableAddressRegion();
	const VkStridedDeviceAddressRegionKHR closestHitTable = shaderTable->GetClosestHitTableAddressRegion();
	const VkStridedDeviceAddressRegionKHR missTable = shaderTable->GetMissTableAddressRegion();
	const VkStridedDeviceAddressRegionKHR emptyTable{};

	RendererVk<Target>::pvkCmdTraceRaysKHR(m_commandBuffers[_GetCommandListIndex()],
						&raygenTable, &missTable, &closestHitTable, &emptyTable, 
						resolution.x, resolution.y, 1);
}

template <VulkanTarget Target>
void CommandListVk<Target>::DrawMeshShader(const Vector3ui& groupCount) {
	RendererVk<Target>::pvkCmdDrawMeshTasksEXT(m_commandBuffers[_GetCommandListIndex()], groupCount.x, groupCount.y, groupCount.z);
}

template <VulkanTarget Target>
void CommandListVk<Target>::DispatchCompute(const Vector3ui& groupCount) {
	vkCmdDispatch(m_commandBuffers[_GetCommandListIndex()], groupCount.x, groupCount.y, groupCount.z);
}

template <VulkanTarget Target>
void CommandListVk<Target>::BindGraphicsPipeline(const IGraphicsPipeline& pipeline) {
	vkCmdBindPipeline(m_commandBuffers[_GetCommandListIndex()], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.As<GraphicsPipelineVk<Target>>()->GetPipeline());
}

template <VulkanTarget Target>
void CommandListVk<Target>::BindComputePipeline(const IComputePipeline& pipeline) {
	vkCmdBindPipeline(m_commandBuffers[_GetCommandListIndex()], VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.As<ComputePipelineVk<Target>>()->GetPipeline());
}

template <VulkanTarget Target>
void CommandListVk<Target>::BindRayTracingPipeline(const IRaytracingPipeline& pipeline) {
	vkCmdBindPipeline(m_commandBuffers[_GetCommandListIndex()], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, pipeline.As<RaytracingPipelineVk>()->GetPipeline());
}

template <VulkanTarget Target>
void CommandListVk<Target>::BindMeshPipeline(const IMeshPipeline& meshPipeline) {
	vkCmdBindPipeline(m_commandBuffers[_GetCommandListIndex()], VK_PIPELINE_BIND_POINT_GRAPHICS, meshPipeline.As<MeshPipelineVk>()->GetPipeline());
}

template <VulkanTarget Target>
void CommandListVk<Target>::SetViewport(const Viewport& vp) {
	ICommandList::SetViewport(vp);

	VkViewport viewport{};

	viewport.x = static_cast<float>(vp.rectangle.GetRectanglePosition().x);
	viewport.y = static_cast<float>(vp.rectangle.GetRectanglePosition().y + vp.rectangle.GetRectangleSize().y);
	viewport.width = static_cast<float>(vp.rectangle.GetRectangleSize().x);
	viewport.height = -static_cast<float>(vp.rectangle.GetRectangleSize().y);

	viewport.minDepth = vp.minDepth;
	viewport.maxDepth = vp.maxDepth;

	vkCmdSetViewport(m_commandBuffers[_GetCommandListIndex()], 0, 1, &viewport);
}

template <VulkanTarget Target>
void CommandListVk<Target>::SetScissor(const Vector4ui& scissorRect) {
	VkRect2D scissor{};

	scissor.offset = {
		(int32_t)scissorRect.GetRectanglePosition().x,
		(int32_t)scissorRect.GetRectanglePosition().y
	};

	scissor.extent = {
		scissorRect.GetRectangleSize().x,
		scissorRect.GetRectangleSize().y
	};
	
	vkCmdSetScissor(m_commandBuffers[_GetCommandListIndex()], 0, 1, &scissor);
}

template <VulkanTarget Target>
void CommandListVk<Target>::SetDebugName(const std::string& name) {
	VkDebugUtilsObjectNameInfoEXT nameInfo{};
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType = VK_OBJECT_TYPE_COMMAND_BUFFER;
	nameInfo.pNext = nullptr;

	for (UIndex64 i = 0; i < m_commandBuffers.size(); i++) {
		std::string nname = name + "[" + std::to_string(i) + "]";
		nameInfo.pObjectName = nname.c_str();
		nameInfo.objectHandle = (uint64_t)m_commandBuffers[i];

		if (RendererVk<Target>::pvkSetDebugUtilsObjectNameEXT != nullptr)
			RendererVk<Target>::pvkSetDebugUtilsObjectNameEXT(m_logicalDevice, &nameInfo);
	}
}

template <VulkanTarget Target>
void CommandListVk<Target>::AddDebugMarker(const std::string& mark, const Color& color) {
	VkDebugUtilsLabelEXT label{};
	label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
	label.pLabelName = mark.c_str();
	label.color[0] = color.red;
	label.color[1] = color.green;
	label.color[2] = color.blue;
	label.color[3] = color.alpha;
	label.pNext = nullptr;

	RendererVk<Target>::pvkCmdInsertDebugUtilsLabelEXT(m_commandBuffers[_GetCommandListIndex()], &label);
}

template <VulkanTarget Target>
void CommandListVk<Target>::StartDebugSection(const std::string& mark, const Color& color) {
	VkDebugUtilsLabelEXT label{};
	label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
	label.pLabelName = mark.c_str();
	label.color[0] = color.red;
	label.color[1] = color.green;
	label.color[2] = color.blue;
	label.color[3] = color.alpha;
	label.pNext = nullptr;

	RendererVk<Target>::pvkCmdBeginDebugUtilsLabelEXT(m_commandBuffers[_GetCommandListIndex()], &label);
}

template <VulkanTarget Target>
void CommandListVk<Target>::EndDebugSection() {
	RendererVk<Target>::pvkCmdEndDebugUtilsLabelEXT(m_commandBuffers[_GetCommandListIndex()]);
}

#endif
