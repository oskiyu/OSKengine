#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

constexpr auto VK_FLAGS_NONE = 0;

constexpr auto DEFAULT_FENCE_TIMEOUT = 1000000000;

/*errorcode -> string*/
std::string errorString(VkResult errorCode);

//Device type -> string
std::string physicalDeviceTypeString(VkPhysicalDeviceType type);

//
VkBool32 getSupportedDepthFormat(VkPhysicalDevice device, VkFormat* format);

void setImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageLayout oldImgLayout, VkImageLayout newImgLayout, VkImageSubresourceRange subresourceRange, VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

void setImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImgLayout, VkImageLayout newImgLayout, VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

void insertImageMemoryBarrier(VkCommandBuffer cmdBuffer, VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageSubresourceRange subresourceRange);

void exitFatal(std::string message, int32_t exitCode);
void exitFatal(std::string message, VkResult resultCode);

VkShaderModule loadShader(const char* fileName, VkDevice device);

bool fileExists(const std::string& filename);


struct SwapChainSupportDetails {

	VkSurfaceCapabilitiesKHR capabilities;

	std::vector<VkSurfaceFormatKHR> formats;

	std::vector<VkPresentModeKHR> presentModes;

};

