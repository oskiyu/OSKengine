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

template <VulkanTarget Target>
RenderpassVk<Target>::RenderpassVk(
	const DynamicArray<RenderpassAttachmentVk>& attachments,
	const DynamicArray<RenderSubpassVk>& subpasses)
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

	const auto device = Engine::GetRenderer()->GetGpu()->As<GpuVk<Target>>()->GetLogicalDevice();
	vkCreateRenderPass(
		device,
		&renderPassInfo,
		nullptr,
		&m_renderpass);
}

template <VulkanTarget Target>
RenderpassVk<Target>::~RenderpassVk() {
	if (m_renderpass) {
		const auto device = Engine::GetRenderer()->GetGpu()->As<GpuVk<Target>>()->GetLogicalDevice();
		vkDestroyRenderPass(
			device,
			m_renderpass,
			nullptr);
	}
}

template <VulkanTarget Target>
RenderpassVk<Target>::RenderpassVk(RenderpassVk<Target>&& other) noexcept {
	std::swap(m_renderpass, other.m_renderpass);
}

template <VulkanTarget Target>
const RenderpassVk<Target>& RenderpassVk<Target>::operator=(RenderpassVk<Target>&& other) noexcept {
	std::swap(m_renderpass, other.m_renderpass);
	return *this;
}

template <VulkanTarget Target>
VkRenderPass RenderpassVk<Target>::GetRenderpass() const {
	return m_renderpass;
}

#endif //  OSK_USE_VULKAN_BACKEND
