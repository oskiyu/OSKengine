#pragma once

#include <vulkan/vulkan.h>
#include <string>

#include "Vertex.h"
#include "Vector4.h"
#include <vector>

#include "OSKtypes.h"

namespace OSK::VULKAN {
	class Renderpass;
}

namespace OSK {

	class GraphicsPipeline {

		friend class VulkanRenderer;

	public:

		~GraphicsPipeline();

		void SetViewport(const Vector4& size, const Vector2& depthMinMax = { 0.0f, 1.0f });

		void SetRasterizer(const VkBool32& renderObjectsOutsideRange, VkPolygonMode polygonMode, VkCullModeFlagBits cullMode, VkFrontFace frontFaceType);

		void SetMSAA(VkBool32 use, VkSampleCountFlagBits samples);

		void SetDepthStencil(bool use); 

		void SetPushConstants(VkShaderStageFlagBits shaderStage, uint32_t size, uint32_t offset = 0);

		void SetLayout(VkDescriptorSetLayout* layout);

		OskResult Create(VULKAN::Renderpass* renderpass);

		void Bind(VkCommandBuffer commandBuffer) const;

		OskResult ReloadShaders();

		VkPipeline VulkanPipeline;
		VkPipelineLayout VulkanPipelineLayout;

	private:

		GraphicsPipeline(VkDevice logicalDevice, const std::string& vertexPath, const std::string& indexPath);

		bool viewportHasBeenSet = false;
		bool rasterizerHasBeenSet = false;
		bool msaaHasBeenSet = false;
		bool depthStencilHasBeenSet = false;
		bool pushConstantsHaveBeenSet = false;
		bool layoutHasBeenSet = false;

		std::string vertexPath;
		std::string fragmentPath;

		VkShaderModule createShaderModule(const std::vector<char>& code) const;

		OskResult loadShaders();

		void clearShaders();

		VkDevice logicalDevice;

		VkPipelineShaderStageCreateInfo shaderStages[2] = {};
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		VkViewport viewport{};
		VkRect2D scissor{};
		VkPipelineViewportStateCreateInfo viewportState{};

		VkPipelineRasterizationStateCreateInfo rasterizer{};

		VkPipelineMultisampleStateCreateInfo multisampling{};

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo colorBlending{};

		VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};

		VkPushConstantRange pushConstRange{};
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

		VkDescriptorSetLayout* descriptorSetLayout;

		VkGraphicsPipelineCreateInfo pipelineInfo{};

		VkShaderModule vertexShaderModule;
		VkShaderModule fragmentShaderModule;

		VkVertexInputBindingDescription vertexBindingDesc;
		std::array<VkVertexInputAttributeDescription, OSK_VERTEX_ATTRIBUTES_COUNT> vertexInputDesc;

		VULKAN::Renderpass* targetRenderpass = nullptr;

	};

}