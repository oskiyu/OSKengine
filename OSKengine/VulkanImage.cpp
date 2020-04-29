#include "VulkanImage.h"

#include "VulkanDevice.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDescriptorSet.h"
#include "VulkanCommandPool.h"

namespace OSK::VULKAN {

	void VulkanImage::CreateImage(const VulkanDevice& device, const uint32_t& sizeX, const uint32_t& sizeY, const uint32_t& mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
		//Struct para crear la imagen
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = sizeX;
		imageInfo.extent.height = sizeY;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		//Crear la imagen y error-handling
		VkResult result = vkCreateImage(device.LogicalDevice, &imageInfo, NULL, &Image);
		if (result != VK_SUCCESS) {
			OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "crear imagen", __LINE__);
			throw std::runtime_error("ERROR: crear imagen");
		}

		//Alloc de memoria
		VkMemoryRequirements memReq;
		vkGetImageMemoryRequirements(device.LogicalDevice, Image, &memReq);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memReq.size;
		allocInfo.memoryTypeIndex = device.FindMemoryType(memReq.memoryTypeBits, properties);

		//Alloc memoria y error-handling
		result = vkAllocateMemory(device.LogicalDevice, &allocInfo, NULL, &ImageMemory);
		if (result != VK_SUCCESS) {
			OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "alloc imagen", __LINE__);
			throw std::runtime_error("ERROR: alloc imagen");
		}

		vkBindImageMemory(device.LogicalDevice, Image, ImageMemory, 0);
	}


	void VulkanImage::LoadImage(const VulkanDevice& device, VulkanCommandPool* commandPool, VkQueue Q, const std::string& path) {

		//Buffers
		VulkanBuffer stagingBuffer{};

		//Carga la textura
		stbi_uc* pixels = stbi_load(path.c_str(), &SizeX, &SizeY, &NrChannels, STBI_rgb_alpha);

		//Mipmaps
		MipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(SizeX, SizeY)))) + 1;

		//Alloc memoria para la textura
		VkDeviceSize imageSize = SizeX * SizeY * 4;

		//Error check
		if (!pixels) {
			throw std::runtime_error("ERROR: load texture: ");
		}

		//Crea los buffers para la textura
		stagingBuffer.CreateBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		//Mete los datos de la textura al buffer
		void* data;
		vkMapMemory(device.LogicalDevice, stagingBuffer.BufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(device.LogicalDevice, stagingBuffer.BufferMemory);

		//Clear
		stbi_image_free(pixels);

		//Crear VkImage
		CreateImage(device, SizeX, SizeY, MipLevels, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		TransictionImageLayout(device, commandPool, Q, Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, MipLevels);

		CopyBufferToImage(device, stagingBuffer, commandPool, Q);

		stagingBuffer.DestroyBuffer(device);

		GenerateMipmaps(device, commandPool, VK_FORMAT_R8G8B8A8_UNORM, Q);

		ImageView = CreateImageView(device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

		CreateTextureSampler(device);
	}


	OSK_INFO_STATIC
	VkImageView VulkanImage::CreateImageView(const VulkanDevice& device, VkImage image, VkFormat format, VkImageAspectFlags aspect, const uint32_t& mipLevels) {
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspect;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;

		//Creacion y error-handling
		VkResult result = vkCreateImageView(device.LogicalDevice, &viewInfo, NULL, &imageView);
		if (result != VK_SUCCESS) {
			OSK::Logger::Log(OSK::LogMessageLevels::BAD_ERROR, "create texture view", __LINE__);
		}

		return imageView;
	}


	VkImageView VulkanImage::CreateImageView(const VulkanDevice& device, VkFormat format, VkImageAspectFlags aspect) {
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = Image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspect;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = MipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;

		//Creacion y error-handling
		VkResult result = vkCreateImageView(device.LogicalDevice, &viewInfo, NULL, &imageView);
		if (result != VK_SUCCESS) {
			OSK::Logger::Log(OSK::LogMessageLevels::BAD_ERROR, "create texture view", __LINE__);
		}

		return imageView;
	}


	void VulkanImage::TransictionImageLayout(const VulkanDevice& device, VulkanCommandPool* commandPool, VkQueue Q, VkImage image, VkFormat format, VkImageLayout old, VkImageLayout newL, uint32_t mipLevels) {
		VulkanCommandBuffer commandBuffer{};
		commandBuffer.BeginSingleTimeCommand(device, commandPool);

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = old;
		barrier.newLayout = newL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0; // TODO
		barrier.dstAccessMask = 0; // TODO

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (old == VK_IMAGE_LAYOUT_UNDEFINED && newL == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (old == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newL == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(commandBuffer.CommandBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier);

		commandBuffer.EndSingleTimeCommand(device, Q);
	}


	void VulkanImage::CreateTextureSampler(const VulkanDevice& device) {
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;

		/* Tipos:

		VK_FILTER_LINEAR: 'aa'

		VK_FILTER_NEAREST: 'pixelado'

		*/

		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float_t>(MipLevels);

		//Crear el sampler y error-handling
		VkResult result = vkCreateSampler(device.LogicalDevice, &samplerInfo, NULL, &ImageSampler);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("ERROR: texture sampler");
		}
	}


	void VulkanImage::CopyBufferToImage(const VulkanDevice& device, const VulkanBuffer& buffer, VulkanCommandPool* commandPool, VkQueue Q) {
		VulkanCommandBuffer cbuffer{};
		cbuffer.BeginSingleTimeCommand(device, commandPool);

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { (uint32_t)SizeX, (uint32_t)SizeY, 1 };

		vkCmdCopyBufferToImage(cbuffer.CommandBuffer, buffer.Buffer, Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		cbuffer.EndSingleTimeCommand(device, Q);
	}


	void VulkanImage::GenerateMipmaps(const VulkanDevice& device, VulkanCommandPool* commandPool, VkFormat format, VkQueue Q) {
		//Comporobar que está soportado por la gpu
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(device.PhysicalDevice, format, &formatProperties);

		//Error-handlling
		bool result = formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
		if (!result) {
			OSK::Logger::Log(OSK::LogMessageLevels::BAD_ERROR, "mipmap no soportados", __LINE__);
			throw std::runtime_error("ERROR: mipmap no soportados");
		}

		VulkanCommandBuffer commandBuffer{};
		commandBuffer.BeginSingleTimeCommand(device, commandPool);

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = Image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipSizeX = SizeX;
		int32_t mipSizeY = SizeY;

		for (uint32_t i = 1; i < MipLevels; i++) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer.CommandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit blit = {};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipSizeX, mipSizeY, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipSizeX > 1 ? mipSizeX / 2 : 1, mipSizeY > 1 ? mipSizeY / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer.CommandBuffer,
				Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer.CommandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			if (mipSizeX > 1)
				mipSizeX /= 2;
			if (mipSizeY > 1)
				mipSizeY /= 2;
		}

		barrier.subresourceRange.baseMipLevel = MipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer.CommandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, NULL,
			0, NULL,
			1, &barrier);


		commandBuffer.EndSingleTimeCommand(device, Q);
	}


	void VulkanImage::Destroy(const VulkanDevice& device) {
		//Cleanup texturas
		vkDestroySampler(device.LogicalDevice, ImageSampler, NULL);
		vkDestroyImageView(device.LogicalDevice, ImageView, NULL);
		vkDestroyImage(device.LogicalDevice, Image, NULL);
		vkFreeMemory(device.LogicalDevice, ImageMemory, NULL);
	}

}