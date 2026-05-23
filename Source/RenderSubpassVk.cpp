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

DynamicArray<VkSubpassDependency> RenderSubpassVk::GetDependencies() const {
	auto output = DynamicArray<VkSubpassDependency>::CreateResized(1);

	output[0].srcSubpass = VK_SUBPASS_EXTERNAL; // "none"
	output[0].dstSubpass = 0; // Self

	output[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT; // ?
	output[0].srcAccessMask = 0;

	output[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT; // ?
	output[0].dstStageMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; // ?

	return output;
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
