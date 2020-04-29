#pragma once

#include <vulkan/vulkan.h>

#include "FileIO.h"

#include "Vertex.h"

namespace OSK::VULKAN {

	struct VulkanDevice;

	struct VulkanSwapchain;

	struct VulkanRenderpass;

	struct VulkanGraphicsPipeline {

		VkPipelineLayout PipelineLayout;

		
		VkPipeline GraphicsPipeline;

		
		void CreateGraphicsPipeline(const VulkanDevice& device, const VulkanSwapchain& swapchain, const VulkanRenderpass& renderpass, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

		
		void Destroy(const VulkanDevice& device);


		//Crea un shader module a partir de un codigo spv
		static VkShaderModule CreateShaderModule(const VulkanDevice& device, const std::vector<char>& code);

	};

}