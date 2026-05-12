#include "FramebufferVk.h"

#ifdef OSK_USE_VULKAN_BACKEND

#include "Assert.h"
#include "Exceptions.h"

#include "RenderpassVk.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

FramebufferVk::FramebufferVk(const GpuVk& device, const RenderpassVk& renderpass, std::span<const GpuImageViewVk* const> views) {
	m_device = device.GetLogicalDevice();

	auto v = DynamicArray<VkImageView>::CreateResized(views.size());
	for (UIndex64 i = 0; i < views.size(); i++) {
		v[i] = views[i]->GetVkView();
	}

	VkFramebufferCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	info.renderPass = renderpass.GetRenderpass();
	info.attachmentCount = static_cast<USize32>(v.GetSize());
	info.pAttachments = v.GetData();
	info.width = views[0]->GetSize2D().x;
	info.height = views[0]->GetSize2D().y;
	info.layers = 1;

	const auto result = vkCreateFramebuffer(m_device, &info, nullptr, &m_framebuffer);
	OSK_ASSERT(result == VK_SUCCESS, EngineException(std::format("Error al crear el framebuffer: {}", (int)result)));
}

FramebufferVk::~FramebufferVk() {
	if (m_framebuffer) {
		vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
		m_framebuffer = nullptr;
	}
}

FramebufferVk::FramebufferVk(FramebufferVk&& other) noexcept {
	m_framebuffer = other.m_framebuffer;
	m_device = other.m_device;
	other.m_framebuffer = nullptr;
}

FramebufferVk& FramebufferVk::operator=(FramebufferVk&& other) noexcept  {
	std::swap(m_framebuffer, other.m_framebuffer);
	return *this;
}


VkFramebuffer FramebufferVk::GetFramebuffer() const {
	return m_framebuffer;
}

#endif // OSK_USE_VULKAN_BACKEND
