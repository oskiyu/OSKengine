#include "Framebuffer.h"

#include "VulkanRenderer.h"

using namespace OSK::VULKAN;

Framebuffer::~Framebuffer() {
	Clear();
}

Framebuffer::Framebuffer(OSK::RenderAPI* renderer) {
	this->renderer = renderer;
}

void Framebuffer::AddImageView(VULKAN::GPUImage* image) {
	attachments.push_back(image->View);
}

void Framebuffer::AddImageView(VkImageView view) {
	attachments.push_back(view);
}

void Framebuffer::Create(VULKAN::Renderpass* renderpass, uint32_t sizeX, uint32_t sizeY) {
	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderpass->VulkanRenderpass;
	framebufferInfo.attachmentCount = attachments.size();
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = sizeX;
	framebufferInfo.height = sizeY;
	framebufferInfo.layers = 1;

	size = { sizeY, sizeY };

	VkResult result = vkCreateFramebuffer(renderer->LogicalDevice, &framebufferInfo, nullptr, &framebuffer);
	if (result != VK_SUCCESS)
		throw std::runtime_error("ERROR: crear framebuffer.");

	isActive = true;
}

void Framebuffer::Clear() {
	if (isActive)
		vkDestroyFramebuffer(renderer->LogicalDevice, framebuffer, nullptr);

	isActive = false;
}