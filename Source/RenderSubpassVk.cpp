#include "RenderSubpassVk.h"

#ifdef OSK_USE_VULKAN_BACKEND

using namespace OSK::GRAPHICS;

RenderSubpassVk::RenderSubpassVk(
	VkPipelineBindPoint bindPoint,
	const DynamicArray<RenderpassAttachmentVk>& colorAttachments,
	const RenderpassAttachmentVk& depthAttachment) 
	: m_depthAttachment(depthAttachment)
{
	m_description.pipelineBindPoint = bindPoint;

	SetColorAttachments(colorAttachments);

	m_description.pDepthStencilAttachment = &m_depthAttachment.GetReference();
}

void RenderSubpassVk::SetColorAttachments(const DynamicArray<RenderpassAttachmentVk>& attachments) {
	/// TODO: check overflow
	m_description.colorAttachmentCount = static_cast<uint32_t>(attachments.GetSize());

	for (const auto& attachment : attachments) {
		m_references.Insert(attachment.GetReference());
	}

	m_description.pColorAttachments = m_references.GetData();
}

#endif // OSK_USE_VULKAN_BACKEND
