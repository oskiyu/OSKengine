#include "RenderpassVk.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "GpuVk.h"

#ifdef  OSK_USE_VULKAN_BACKEND

using namespace OSK;
using namespace OSK::GRAPHICS;

DynamicArray<VkAttachmentDescription> GetAttachmentsVk(const DynamicArray<RenderpassAttachmentVk>& attachments) {
	auto output = DynamicArray<VkAttachmentDescription>::CreateResized(attachments.GetSize());

	for (UIndex64 i = 0; i < attachments.GetSize(); i++) {
		output[i] = attachments[i].GetDescription();
	}

	return output;
}

DynamicArray<VkSubpassDescription> GetSubpassesVk(const DynamicArray<RenderSubpassVk>& subpasses) {
	auto output = DynamicArray<VkSubpassDescription>::CreateResized(subpasses.GetSize());

	for (UIndex64 i = 0; i < subpasses.GetSize(); i++) {
		output[i] = subpasses[i].GetDescription();
	}

	return output;
}

DynamicArray<VkSubpassDependency> GetDependenciesVk(const DynamicArray<RenderSubpassVk>& subpasses) {
	DynamicArray<VkSubpassDependency> output{};

	for (const auto& subpass : subpasses) {
		output.InsertAll(subpass.GetDependencies());
	}

	return output;
}

RenderpassAttachmentVk GetColorAttachment() {
	return RenderpassAttachmentVk(
		VK_FORMAT_MAX_ENUM,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);
}

RenderSubpassVk GetColorSubpass() {
	return RenderSubpassVk(
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ GetColorAttachment() },
		GetColorAttachment()
	);
}

RenderpassVk::RenderpassVk(
	const GpuVk* gpu,
	const DynamicArray<RenderpassAttachmentVk>& attachments,
	const DynamicArray<RenderSubpassVk>& subpasses)
	:
	m_framebuffers(gpu)
{
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	const auto attachmentsVk = GetAttachmentsVk(attachments);
	renderPassInfo.attachmentCount = attachmentsVk.GetSize();
	renderPassInfo.pAttachments = attachmentsVk.GetData();

	const auto subpassesVk = GetSubpassesVk(subpasses);
	renderPassInfo.subpassCount = subpassesVk.GetSize();
	renderPassInfo.pSubpasses = subpassesVk.GetData();

	const auto dependenciesVk = GetDependenciesVk(subpasses);
	renderPassInfo.dependencyCount = dependenciesVk.GetSize();
	renderPassInfo.pDependencies = dependenciesVk.GetData();

	const auto device = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();
	const auto result = vkCreateRenderPass(
		device,
		&renderPassInfo,
		nullptr,
		&m_renderpass);
	OSK_ASSERT(result == VK_SUCCESS, EngineException(std::format("Error al crear el renderpass: {}", (int)result)));
}

RenderpassVk::~RenderpassVk() {
	if (m_renderpass) {
		const auto device = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();
		vkDestroyRenderPass(
			device,
			m_renderpass,
			nullptr);
	}
}

RenderpassVk::RenderpassVk(RenderpassVk&& other) noexcept : m_framebuffers(nullptr) {
	std::swap(m_renderpass, other.m_renderpass);
	std::swap(m_framebuffers, other.m_framebuffers);
}

RenderpassVk& RenderpassVk::operator=(RenderpassVk&& other) noexcept {
	std::swap(m_renderpass, other.m_renderpass);
	std::swap(m_framebuffers, other.m_framebuffers);
	return *this;
}

VkRenderPass RenderpassVk::GetRenderpass() const {
	return m_renderpass;
}

FramebufferVk* RenderpassVk::GetFramebuffer(const FramebufferVkMapEntry& entry) {
	return m_framebuffers.GetFramebuffer(entry, *this);
}

#endif //  OSK_USE_VULKAN_BACKEND
