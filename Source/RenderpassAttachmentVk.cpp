#include "RenderpassAttachmentVk.h"

#ifdef OSK_USE_VULKAN_BACKEND

using namespace OSK::GRAPHICS;

RenderpassAttachmentVk::RenderpassAttachmentVk(
	VkFormat targetFormat,
	VkAttachmentLoadOp loadOp,
	VkAttachmentStoreOp storeOp,
	VkImageLayout startLayout,
	VkImageLayout endLayout)
{
	m_description.format = targetFormat;

	m_description.loadOp = loadOp;
	m_description.storeOp = storeOp;
	
	m_description.stencilLoadOp = loadOp;
	m_description.stencilStoreOp = storeOp;

	m_description.initialLayout = startLayout;
	m_description.finalLayout = endLayout;

	m_description.samples = VK_SAMPLE_COUNT_1_BIT;
}

VkAttachmentDescription RenderpassAttachmentVk::GetDescription() const {
	return m_description;
}

#endif // OSK_USE_VULKAN_BACKEND
