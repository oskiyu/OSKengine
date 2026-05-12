#include "RenderSubpassVk.h"

#ifdef OSK_USE_VULKAN_BACKEND

using namespace OSK;
using namespace OSK::GRAPHICS;

RenderSubpassVk::RenderSubpassVk(
	VkPipelineBindPoint bindPoint,
	const DynamicArray<RenderpassAttachmentVk>& colorAttachments,
	std::optional<RenderpassAttachmentVk> depthAttachment)
	: m_depthAttachment(depthAttachment)
{
	m_description.pipelineBindPoint = bindPoint;

	SetColorAttachments(colorAttachments);
	
	m_depthReference.attachment = m_description.colorAttachmentCount + 1;
	m_depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	m_description.pDepthStencilAttachment = depthAttachment ? &m_depthReference : nullptr;
}

VkSubpassDescription RenderSubpassVk::GetDescription() const {
	return m_description;
}

const DynamicArray<VkSubpassDependency>& RenderSubpassVk::GetDependencies() const {
	return m_dependencies;
}

void RenderSubpassVk::SetColorAttachments(const DynamicArray<RenderpassAttachmentVk>& attachments) {
	m_description.colorAttachmentCount = static_cast<uint32_t>(attachments.GetSize());

	for (UIndex32 i = 0; i < m_description.colorAttachmentCount; i++) {
		VkAttachmentReference reference{};
		reference.attachment = i;
		reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		m_references.Insert(reference);
	}
	
	m_description.pColorAttachments = m_references.GetData();
}

#endif // OSK_USE_VULKAN_BACKEND
