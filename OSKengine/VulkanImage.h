#pragma once

#include <vulkan/vulkan.h>
#include "stbi_image.h"
#include <string>

#include "VulkanDescriptorSet.h"

#include "Log.h"

namespace OSK::VULKAN {

	struct VulkanDevice;

	struct VulkanBuffer;

	struct VulkanCommandBuffer;

	struct VulkanCommandPool;

	struct VulkanImage {

		VkImage Image;

		VulkanDescriptorSet DescriptorSet;

		VkDeviceMemory ImageMemory;

		VkImageView ImageView;

		VkSampler ImageSampler;

		uint32_t MipLevels = 4;

		int32_t SizeX;

		int32_t SizeY;

		int32_t NrChannels;

		void CreateImage(const VulkanDevice& device, const uint32_t& sizeX, const uint32_t& sizeY, const uint32_t& mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);


		//Despues llamar createdescriptorset.
		void LoadImage(const VulkanDevice& device, VulkanCommandPool* commandPool, VkQueue Q, const std::string& path);


		static VkImageView CreateImageView(const VulkanDevice& device, VkImage image, VkFormat format, VkImageAspectFlags aspect, const uint32_t& mipLevels);
		
		
		VkImageView CreateImageView(const VulkanDevice& device, VkFormat format, VkImageAspectFlags aspect);


		void CreateTextureSampler(const VulkanDevice& device);


		void TransictionImageLayout(const VulkanDevice& device, VulkanCommandPool* commandPool, VkQueue Q, VkImage image, VkFormat format, VkImageLayout old, VkImageLayout newL, uint32_t mipLevels);


		void CopyBufferToImage(const VulkanDevice& device, const VulkanBuffer& buffer, VulkanCommandPool* commandPool, VkQueue Q);


		//
		void GenerateMipmaps(const VulkanDevice& device, VulkanCommandPool* commandPool, VkFormat format, VkQueue Q);


		void Destroy(const VulkanDevice& device);

	};

}