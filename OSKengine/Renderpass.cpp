#include "Renderpass.h"
#include <stdexcept>

namespace OSK::VULKAN {

	void RenderpassAttachment::AddAttachment(VkFormat format, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkImageLayout layout) {
		attahcmentDesc.format = format;
		//attahcmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		//LoadOp: qué se hace al renderizarsobre  una imagen.
		//	VK_ATTACHMENT_LOAD_OP_LOAD: se conserva lo que había antes.
		//	VK_ATTACHMENT_LOAD_OP_CLEAR: se limpia.
		//	VK_ATTACHMENT_LOAD_OP_DONT_CARE: da igual.
		attahcmentDesc.loadOp = loadOp;
		attahcmentDesc.storeOp = storeOp;
		//Stencil.
		attahcmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attahcmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attahcmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attahcmentDesc.finalLayout = layout;
	}

	void RenderpassAttachment::CreateReference(const uint32_t& attachment, VkImageLayout use) {
		reference.attachment = attachment;
		reference.layout = use;
	}


	RenderpassSubpass::~RenderpassSubpass() {
		references.clear();
	}

	void RenderpassSubpass::SetPipelineBindPoint(VkPipelineBindPoint point) {
		description.pipelineBindPoint = point;
	}

	void RenderpassSubpass::SetColorAttachments(const std::vector<RenderpassAttachment>& attachments) {
		description.colorAttachmentCount = attachments.size();
		
		for (uint32_t i = 0; i < attachments.size(); i++) {
			references.push_back(attachments[i].reference);
		}
		description.pColorAttachments = references.data();

	}

	void RenderpassSubpass::SetDepthStencilAttachment(const RenderpassAttachment& attachment) {
		description.pDepthStencilAttachment = &attachment.reference;
	}

	void RenderpassSubpass::Set(const uint32_t& srcSubpass, const uint32_t& dstSubpass, VkPipelineStageFlags sourceStageMask, VkAccessFlags sourceAccess, VkPipelineStageFlags destStageMask, VkAccessFlags destAccess){
		dependency.srcSubpass = srcSubpass;
		dependency.dstSubpass = dstSubpass;
		dependency.srcStageMask = sourceStageMask;
		dependency.srcAccessMask = sourceAccess;
		dependency.dstStageMask = destStageMask;
		dependency.dstAccessMask = destAccess;
	}

	Renderpass::Renderpass(VkDevice logicalDevice) {
		this->logicalDevice = logicalDevice;
	}

	Renderpass::~Renderpass() {
		vkDestroyRenderPass(logicalDevice, VulkanRenderpass, nullptr);
	}

	void Renderpass::SetMSAA(VkSampleCountFlagBits samples) {
		this->samples = samples;
	}

	void Renderpass::AddAttachment(RenderpassAttachment attachment) {
		attachment.attahcmentDesc.samples = samples;
		
		attachments.push_back(attachment);
	}

	void Renderpass::AddSubpass(RenderpassSubpass subpass) {
		subpasses.push_back(subpass);
	}

	void Renderpass::Create() {
		std::vector<VkAttachmentDescription> attachments;
		for (auto& i : this->attachments)
			attachments.push_back(i.attahcmentDesc);

		std::vector<VkSubpassDescription> subpasses;
		for (auto& i : this->subpasses)
			subpasses.push_back(i.description);

		std::vector<VkSubpassDependency> subpassesDependencies;
		for (auto& i : this->subpasses)
			subpassesDependencies.push_back(i.dependency);

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = attachments.size();
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = subpasses.size();
		renderPassInfo.pSubpasses = subpasses.data();
		renderPassInfo.dependencyCount = subpassesDependencies.size();
		renderPassInfo.pDependencies = subpassesDependencies.data();

		VkResult result = vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &VulkanRenderpass);
		if (result != VK_SUCCESS)
			throw std::runtime_error("ERROR: crear renderpass.");
	}

}