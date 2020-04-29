#include "VulkanRenderpass.h"

#include "VulkanSwapchain.h"
#include "VulkanDevice.h"

namespace OSK::VULKAN {

	void VulkanRenderpass::CreateRenderpass(const VulkanDevice& device, const VulkanSwapchain& swapchain) {
		//Color attachment
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = swapchain.SwapchainImageFormat; //Formato de imagen
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; //Multisampling?

		/* Modos de colorAttachment.loadOp:

			'VK_ATTACHMENT_LOAD_OP_LOAD' = preserva la información del framebuffer

			'VK_ATTACHMENT_LOAD_OP_CLEAR' = clear

			'VK_ATTACHMENT_LOAD_OP_DONT_CARE' = no se hace nada
		*/
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //Datos antes de renderizar

		/* Modos de colorAttachment.storeOp:

			'VK_ATTACHMENT_STORE_OP_STORE' = se guarda la información (para mostrarla)

			'VK_ATTACHMENT_STORE_OP_DONT_CARE' =  no se hace nada
		*/
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; //Datos después de renderizar
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = FindDepthFormat(device);
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		//Subpasses
		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		//Dependencias del subpass
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		//Crear el renderpass
		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO; //Tipo de struct
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VkResult result = vkCreateRenderPass(device.LogicalDevice, &renderPassInfo, NULL, &RenderPass);

		//Error handling
		if (result != VK_SUCCESS) {
			OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "crear render pass", __LINE__);

			throw std::runtime_error("ERROR: crear render pass");
		}
	}


	void VulkanRenderpass::Destroy(const VulkanDevice& device) {
		vkDestroyRenderPass(device.LogicalDevice, RenderPass, NULL);
	}


	VkFormat VulkanRenderpass::FindDepthFormat(const VulkanDevice& device) {
		return FindSupportedFormat(device, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}


	VkFormat VulkanRenderpass::FindSupportedFormat(const VulkanDevice& device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			VkFormatProperties properties;
			vkGetPhysicalDeviceFormatProperties(device.PhysicalDevice, format, &properties);

			if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
				return format;
			}

		}

		//Error-handling
		OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "FindSupportedFormat", __LINE__);
		throw std::runtime_error("ERROR: FindSupportedFormat");
	}

}