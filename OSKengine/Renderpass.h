#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace OSK::VULKAN {
	//TODO: stencil.

	struct RenderpassAttachment {

		void AddAttachment(VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp stroeOp, VkImageLayout layout);

		void CreateReference(const uint32_t& attachment, VkImageLayout use);

		VkAttachmentDescription attahcmentDesc;
		VkAttachmentReference reference;

	};

	struct RenderpassSubpass {

		~RenderpassSubpass();

		void SetPipelineBindPoint(VkPipelineBindPoint point = VK_PIPELINE_BIND_POINT_GRAPHICS);

		void SetColorAttachments(const std::vector<RenderpassAttachment>& attachments);
		void SetDepthStencilAttachment(const RenderpassAttachment& attachment);

		void Set(const uint32_t& srcSubpass, const uint32_t& dstSubpass, VkPipelineStageFlags sourceStageMask, VkAccessFlags sourceAccess, VkPipelineStageFlags destStageMask, VkAccessFlags destAccess);

		VkSubpassDependency dependency{};
		VkSubpassDescription description;

		std::vector<VkAttachmentReference> references{};

	};

	class Renderpass {

		friend class GraphicsPipeline;
		friend class VulkanRenderer;

	public:

		~Renderpass();

		void SetMSAA(VkSampleCountFlagBits samples);

		void AddAttachment(RenderpassAttachment attachment);

		void AddSubpass(RenderpassSubpass subpass);

		void Create();

		VkRenderPass VulkanRenderpass;

	private:

		Renderpass(VkDevice logicalDevice);

		//Config:
		std::vector<RenderpassAttachment> attachments{};

		std::vector<RenderpassSubpass> subpasses;

		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

		VkDevice logicalDevice;
		
		VkFormat swapchainFormat;

	};

}