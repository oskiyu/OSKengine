#include "VulkanImageGen.h"

#include "VulkanRenderer.h"

namespace OSK::VULKAN {

	VulkanRenderer* VulkanImageGen::renderer = nullptr;

	void VulkanImageGen::SetRenderer(VulkanRenderer* renderer) {
		VulkanImageGen::renderer = renderer;
	}

	void VulkanImageGen::CreateImage(VULKAN::VulkanImage* image, const Vector2ui& size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, const uint32_t& arrayLevels, VkImageCreateFlagBits flags, const uint32_t& mipLevels) {
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(size.X);
		imageInfo.extent.height = static_cast<uint32_t>(size.Y);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = arrayLevels;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = flags;

		VkResult result = vkCreateImage(renderer->LogicalDevice, &imageInfo, nullptr, &image->Image);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear imagen.");

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(renderer->LogicalDevice, image->Image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = renderer->getMemoryType(memRequirements.memoryTypeBits, properties);

		result = vkAllocateMemory(renderer->LogicalDevice, &allocInfo, nullptr, &image->Memory);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: alloc memoria de la imagen.");

		vkBindImageMemory(renderer->LogicalDevice, image->Image, image->Memory, 0);
		image->logicalDevice = &renderer->LogicalDevice;
	}

	void VulkanImageGen::CreateImageSampler(VULKAN::VulkanImage& image, VkFilter filter, VkSamplerAddressMode addressMode, const uint32_t& mipLevels) {
		//Info del sampler.
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		//Filtro:
		//	VK_FILTER_LINEAR: suavizado.
		//	VK_FILTER_NEAREST: pixelado.
		samplerInfo.minFilter = filter;
		samplerInfo.magFilter = filter;
		//AddressMode: como se accede a la imagen con TexCoords fuera de los límites.
		samplerInfo.addressModeU = addressMode;
		samplerInfo.addressModeV = addressMode;
		samplerInfo.addressModeW = addressMode;

		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16.0f;

		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

		samplerInfo.unnormalizedCoordinates = VK_FALSE;

		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = mipLevels;
		samplerInfo.mipLodBias = 0.0f;

		VkResult result = vkCreateSampler(renderer->LogicalDevice, &samplerInfo, nullptr, &image.Sampler);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear sampler." + std::to_string(result));
	}

	void VulkanImageGen::CreateMipmaps(VULKAN::VulkanImage& image, const Vector2ui& size, const uint32_t& levels) {
		VkCommandBuffer cmdBuffer = renderer->beginSingleTimeCommandBuffer();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image.Image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = size.X;
		int32_t mipHeight = size.Y;

		for (uint32_t i = 1; i < levels; i++) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(cmdBuffer, image.Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

			if (mipWidth > 1)
				mipWidth /= 2;
			if (mipHeight > 1)
				mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = levels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		renderer->endSingleTimeCommandBuffer(cmdBuffer);
	}

	void VulkanImageGen::CreateImageView(VulkanImage* img, VkFormat format, VkImageAspectFlags aspect, VkImageViewType type, const uint32_t& layerCount, const uint32_t& mipLevels) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = img->Image;
		viewInfo.viewType = type;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspect;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = layerCount;

		VkResult result = vkCreateImageView(renderer->LogicalDevice, &viewInfo, nullptr, &img->View);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear image view.");
	}
		
	void VulkanImageGen::CopyBufferToImage(VulkanBuffer* buffer, VulkanImage* img, const uint32_t& width, const uint32_t& height) {
		VkCommandBuffer cmdBuffer = renderer->beginSingleTimeCommandBuffer();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage(cmdBuffer, buffer->Buffer, img->Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		renderer->endSingleTimeCommandBuffer(cmdBuffer);
	}
	
	void VulkanImageGen::TransitionImageLayout(VulkanImage* img, VkImageLayout oldLayout, VkImageLayout newLayout, const uint32_t& mipLevels, const uint32_t& arrayLevels) {
		VkCommandBuffer cmdBuffer = renderer->beginSingleTimeCommandBuffer();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = img->Image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = arrayLevels;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = 0;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(cmdBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		renderer->endSingleTimeCommandBuffer(cmdBuffer);
	}

	VulkanImage VulkanImageGen::CreateImageFromBitMap(uint32_t width, uint32_t height, uint8_t* pixels) {
		VulkanImage image{};
		CreateImage(&image, { width, height }, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1);

		VkDeviceSize imageSize = (VkDeviceSize)width * height * 4;
		std::vector<uint8_t> nPixels{};
		for (int i = 0; i < imageSize / 4; i++) {
			nPixels.push_back(pixels[i]);
			nPixels.push_back(pixels[i]);
			nPixels.push_back(pixels[i]);
			nPixels.push_back(pixels[i]);
		}
		VulkanBuffer stagingBuffer{};
		renderer->CreateBuffer(stagingBuffer, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* data;
		vkMapMemory(renderer->LogicalDevice, stagingBuffer.Memory, 0, imageSize, 0, &data);
		memcpy(data, nPixels.data(), static_cast<size_t>(imageSize));
		vkUnmapMemory(renderer->LogicalDevice, stagingBuffer.Memory);

		TransitionImageLayout(&image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 1);
		CopyBufferToImage(&stagingBuffer, &image, width, height);
		TransitionImageLayout(&image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 1);

		renderer->DestroyBuffer(stagingBuffer);

		CreateImageView(&image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);

		return image;
	}

}