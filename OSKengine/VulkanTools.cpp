#include "VulkanTools.h"

#include <vector>
#include "VulkanInitializers.hpp"
#include <iostream>
#include <fstream>

std::string errorString(VkResult errorCode) {
	switch (errorCode) {
#define STR(r) case VK_ ##r: return #r
		STR(NOT_READY);
		STR(TIMEOUT);
		STR(EVENT_SET);
		STR(EVENT_RESET);
		STR(INCOMPLETE);
		STR(ERROR_OUT_OF_HOST_MEMORY);
		STR(ERROR_OUT_OF_DEVICE_MEMORY);
		STR(ERROR_INITIALIZATION_FAILED);
		STR(ERROR_DEVICE_LOST);
		STR(ERROR_MEMORY_MAP_FAILED);
		STR(ERROR_LAYER_NOT_PRESENT);
		STR(ERROR_EXTENSION_NOT_PRESENT);
		STR(ERROR_FEATURE_NOT_PRESENT);
		STR(ERROR_INCOMPATIBLE_DRIVER);
		STR(ERROR_TOO_MANY_OBJECTS);
		STR(ERROR_FORMAT_NOT_SUPPORTED);
		STR(ERROR_SURFACE_LOST_KHR);
		STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
		STR(SUBOPTIMAL_KHR);
		STR(ERROR_OUT_OF_DATE_KHR);
		STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
		STR(ERROR_VALIDATION_FAILED_EXT);
		STR(ERROR_INVALID_SHADER_NV);
#undef STR
	default:
		return "UNKNOWN_ERROR";
	}
}

std::string physicalDeviceTypeString(VkPhysicalDeviceType type) {
	switch (type) {
#define STR(r) case VK_PHYSICAL_DEVICE_TYPE_ ##r: return #r
		STR(OTHER);
		STR(INTEGRATED_GPU);
		STR(DISCRETE_GPU);
		STR(VIRTUAL_GPU);
#undef STR
	default:
		return "UNKNOWN_DEVICE_TYPE";
	}
}

VkBool32 getSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat* depthFormat) {
	//Tipos de formatos de profundidad, de mejor a peor.
	const std::vector<VkFormat> depthFormats = {
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};

	for (auto& format : depthFormats) {
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);

		if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			*depthFormat = format;

			return true;
		}

		return false;
	}
}

//Cambia el layout de la imagen asegurándose de que no está en uso y lo mete en un command buffer.
void setImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageLayout oldImgLayout, VkImageLayout newImgLayout, VkImageSubresourceRange subresourceRange, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask) {
	//Image barrier object.
	VkImageMemoryBarrier imageMemoryBarrier = Init::imageMemoryBarrier();
	imageMemoryBarrier.oldLayout = oldImgLayout;
	imageMemoryBarrier.newLayout = newImgLayout;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange = subresourceRange;

	//masks old actionas that have to be finished before transitioning to the new layout.
	switch (oldImgLayout) {
	case VK_IMAGE_LAYOUT_UNDEFINED:
		imageMemoryBarrier.srcAccessMask = 0;
		break;
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		//Initial layout for linear images, preserves emory content.
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		//Img is a color attachment.
		//Make sure writes to the color buffer have been finished.
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		//Img is a depth/stencil attachment.
		//Make sure writes to the depth/stencilbuffer have been finished.
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		//Img is a transfer source.
		//Make sure all reads from the img have ben finished.
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		//Image is a transfer destination.
		//Make sure any writes to the image have been finished.
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		//Image is read by a shader.
		//Make sure shader reads have been finished.
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;

	default:
		break;
	}

	//Dependency of the new image layout.
	switch (newImgLayout) {
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		//Img will be used as transfer destination.
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		//Transfer source.
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		//Color attachment.
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		//Depth / stencil.
		imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		//Sampler / input attachment.
		if (imageMemoryBarrier.srcAccessMask == 0) {
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		break;
	}

	vkCmdPipelineBarrier(cmdBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

void setImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImgLayout, VkImageLayout newImgLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask ) {
	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = aspectMask;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.layerCount = 1;

	setImageLayout(cmdBuffer, image, oldImgLayout, newImgLayout, subresourceRange, srcStageMask, dstStageMask);
}

void insertImageMemoryBarrier(VkCommandBuffer cmdBuffer, VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageSubresourceRange subresourceRange) {
	VkImageMemoryBarrier imgMemoryBarrier = Init::imageMemoryBarrier();
	imgMemoryBarrier.srcAccessMask = srcAccessMask;
	imgMemoryBarrier.dstAccessMask = dstAccessMask;
	imgMemoryBarrier.oldLayout = oldImageLayout;
	imgMemoryBarrier.newLayout = newImageLayout;
	imgMemoryBarrier.image = image;
	imgMemoryBarrier.subresourceRange = subresourceRange;

	vkCmdPipelineBarrier(cmdBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &imgMemoryBarrier);
}


void exitFatal(std::string message, int32_t exitCode) {
	exit(exitCode);
}

void exitFatal(std::string message, VkResult resultCode) {
	exitFatal(message, (int32_t)resultCode);
}

VkShaderModule loadShader(const char* fileName, VkDevice device) {
	std::ifstream is(fileName, std::ios::binary | std::ios::in | std::ios::ate);

	if (is.is_open()) {
		size_t size = is.tellg();
		is.seekg(0, std::ios::beg);
		char* shaderCode = new char[size];
		is.read(shaderCode, size);
		is.close();

		VkShaderModule shaderModule;
		VkShaderModuleCreateInfo moduleCreateInfo = {};
		moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		moduleCreateInfo.codeSize = size;
		moduleCreateInfo.pCode = (uint32_t*)shaderCode;

		vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderModule);

		delete[] shaderCode;

		return shaderModule;
	}

	return VK_NULL_HANDLE;
}

bool fileExists(const std::string& filename) {
	std::ifstream f(filename.c_str());

	return !f.fail();
}