#include "Renderpass.h"
#include <stdexcept>

namespace OSK::VULKAN {

	Renderpass::Renderpass(VkDevice logicalDevice) {
		this->logicalDevice = logicalDevice;
	}

	Renderpass::~Renderpass() {
		if (logicalDevice && vulkanRenderpass) {
			vkDestroyRenderPass(logicalDevice, vulkanRenderpass, nullptr);

			logicalDevice = VK_NULL_HANDLE;
			vulkanRenderpass = VK_NULL_HANDLE;
		}
	}

	void Renderpass::AddAttachment(RenderpassAttachment attachment) {
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
		for (auto& i : this->subpasses) {
			for (auto& d : i.dependencies)
				subpassesDependencies.push_back(d.vulkanDependency);
		}

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = (uint32_t)attachments.size();
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = (uint32_t)subpasses.size();
		renderPassInfo.pSubpasses = subpasses.data();
		renderPassInfo.dependencyCount = (uint32_t)subpassesDependencies.size();
		renderPassInfo.pDependencies = subpassesDependencies.data();

		VkResult result = vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &vulkanRenderpass);
		OSK_ASSERT(result == VK_SUCCESS, "ERROR: crear renderpass. Code: " + std::to_string(result) + ".");
	}

}