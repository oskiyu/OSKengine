#include "SwapchainVk.h"

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include <vulkan/vulkan.h>
#include "GpuVk.h"
#include "FormatVk.h"
#include "Window.h"
#include "GpuImageVk.h"
#include "Assert.h"
#include "Logger.h"
#include "OSKengine.h"
#include "RendererVk.h"
#include "GpuVk.h"
#include "GpuImageDimensions.h"
#include "GpuImageUsage.h"
#include "PresentMode.h"
#include "CommandQueueVk.h"
#include "ResourcesInFlight.h"

#include "RendererExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

VkPresentModeKHR GetPresentModeVk(PresentMode mode) {
	switch (mode) {
	case OSK::GRAPHICS::PresentMode::VSYNC_OFF:
		return VK_PRESENT_MODE_IMMEDIATE_KHR;
	case OSK::GRAPHICS::PresentMode::VSYNC_ON:
		return VK_PRESENT_MODE_FIFO_KHR;
	case OSK::GRAPHICS::PresentMode::VSYNC_ON_TRIPLE_BUFFER:
		return VK_PRESENT_MODE_MAILBOX_KHR;
	}

	return VK_PRESENT_MODE_MAX_ENUM_KHR;
}

template <VulkanTarget Target>
SwapchainVk<Target>::~SwapchainVk() {
	const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVk<Target>>()->GetLogicalDevice();

	vkDestroySwapchainKHR(device, m_swapchain, nullptr);

	for (UIndex32 i = 0; i < GetImageCount(); i++) {
		auto* image = GetImage(i)->As<GpuImageVk<Target>>();

		vkDestroyImageView(device, image->GetSwapchainView(), nullptr);
		image->_SetVkImage(VK_NULL_HANDLE);
	}
}

template <VulkanTarget Target>
SwapchainVk<Target>::SwapchainVk(PresentMode mode, Format format, const GpuVk<Target>& device, const Vector2ui& resolution, std::span<const UIndex32> queueIndices) : ISwapchain(mode, format) {
	CreationLogic(device, resolution, queueIndices);

	for (auto index : queueIndices) {
		m_queueIndices.Insert(index);
	}
}

template <VulkanTarget Target>
void SwapchainVk<Target>::CreationLogic(const GpuVk<Target>& device, const Vector2ui& resolution, std::span<const UIndex32> queueIndices) {
	auto& info = device.GetInfo();

	// Formato del swapchain.
	VkSurfaceFormatKHR surfaceFormat{};
	surfaceFormat.colorSpace = GetSupportedColorSpace(device);
	surfaceFormat.format = GetFormatVk(GetColorFormat());

	// Modo de pressentación.
	VkPresentModeKHR presentMode = GetPresentModeVk(GetCurrentPresentMode());

	// Tamaño.
	VkExtent2D extent{};
	extent.width = resolution.x;
	extent.height = resolution.y;

	// Número de imágenes en el swapchain.
	// Por alguna razón, pueden salir al revés.
	const auto minImageCount = glm::min(info.swapchainSupportDetails.surfaceCapabilities.minImageCount, info.swapchainSupportDetails.surfaceCapabilities.maxImageCount);
	const auto maxImageCount = glm::max(info.swapchainSupportDetails.surfaceCapabilities.minImageCount, info.swapchainSupportDetails.surfaceCapabilities.maxImageCount);
	if (
		minImageCount <= MAX_RESOURCES_IN_FLIGHT && maxImageCount >= MAX_RESOURCES_IN_FLIGHT) {
		SetNumImagesInFlight(MAX_RESOURCES_IN_FLIGHT);
	}
	else {
		SetNumImagesInFlight(maxImageCount);
	}
	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "Imágenes mínimas:", minImageCount);
	Engine::GetLogger()->Log(IO::LogLevel::L_DEBUG, "Imágenes máximas:", maxImageCount);
	Engine::GetLogger()->InfoLog(std::format("Número de imágenes de swapchain: {}", GetImageCount()));

	// Create info.
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = device.GetSurface();
	createInfo.minImageCount = GetImageCount();
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// Colas.
	// Cómo se maneja el swapchain.
	if (queueIndices.size() == 1) {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	else {
		bool areAllEqual = true;
		const auto firstIndex = queueIndices[0];

		for (const auto& index : queueIndices) {
			if (firstIndex != index) {
				areAllEqual = false;
				break;
			}
		}

		if (areAllEqual) {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueIndices.size());
			createInfo.pQueueFamilyIndices = queueIndices.data();
		}
	}

	createInfo.preTransform = info.swapchainSupportDetails.surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // ¿Debería mostrarse lo que hay detrás de la ventana?
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE; // Si hay algo tapando la ventana, no se renderiza.
	createInfo.oldSwapchain = nullptr;

	// Crearlo y error-handling.
	VkResult result = vkCreateSwapchainKHR(device.GetLogicalDevice(), &createInfo, nullptr, &m_swapchain);
	OSK_ASSERT(result == VK_SUCCESS, SwapchainCreationException("No se ha podido crear el swapchain", result));

	for (UIndex32 i = 0; i < GetImageCount(); i++) {
		GpuImageCreateInfo imageInfo = GpuImageCreateInfo::CreateDefault2D(resolution, GetColorFormat(), GpuImageUsage::COLOR);
		const auto* queue = Engine::GetRenderer()->UseUnifiedCommandQueue()
			? Engine::GetRenderer()->GetUnifiedQueue()
			: Engine::GetRenderer()->GetPresentationQueue();

		SetImage(MakeUnique<GpuImageVk<Target>>(imageInfo, queue), i);
	}

	AcquireImages(device);
	AcquireViews(device);
}


template <VulkanTarget Target>
void SwapchainVk<Target>::AcquireImages(const GpuVk<Target>& device) {
	auto imageCount = GetImageCount();
	VkResult result = vkGetSwapchainImagesKHR(device.GetLogicalDevice(), m_swapchain, &imageCount, nullptr);
	OSK_ASSERT(result == VK_SUCCESS, SwapchainCreationException("Error al adquirir imagenes del swapchain", result));

	auto tempImages = DynamicArray<VkImage>::CreateResized(imageCount);
	vkGetSwapchainImagesKHR(device.GetLogicalDevice(), m_swapchain, &imageCount, tempImages.GetData());
	OSK_ASSERT(result == VK_SUCCESS, SwapchainCreationException("Error al adquirir imagenes del swapchain", result));

	for (UIndex32 i = 0; i < imageCount; i++)
		GetImage(i)->As<GpuImageVk<Target>>()->_SetVkImage(tempImages[i]);
}

template <VulkanTarget Target>
void SwapchainVk<Target>::AcquireViews(const GpuVk<Target>& device) {
	auto tempViews = std::unique_ptr<VkImageView[]>(new VkImageView[GetImageCount()]);

	for (UIndex32 i = 0; i < GetImageCount(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = GetImage(i)->As<GpuImageVk<Target>>()->GetVkImage();
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = GetFormatVk(GetColorFormat());
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkResult result = vkCreateImageView(device.GetLogicalDevice(), &createInfo, nullptr, &tempViews[i]);
		OSK_ASSERT(result == VK_SUCCESS, SwapchainCreationException("Error al crear view de imagen del swapchain", result));

		if (GetImage(i)->As<GpuImageVk<Target>>()->GetSwapchainView() != VK_NULL_HANDLE) {
			vkDestroyImageView(
				Engine::GetRenderer()->GetGpu()->As<GpuVk<Target>>()->GetLogicalDevice(),
				GetImage(i)->As<GpuImageVk<Target>>()->GetSwapchainView(), 0);
		}

		GetImage(i)->As<GpuImageVk<Target>>()->SetSwapchainView(tempViews[i]);
	}
}

template <VulkanTarget Target>
VkSwapchainKHR SwapchainVk<Target>::GetSwapchain() const {
	return m_swapchain;
}

template <VulkanTarget Target>
void SwapchainVk<Target>::Resize(const IGpu& gpu, Vector2ui newResolution) {
	for (UIndex32 i = 0; i < GetImageCount(); i++) {
		GetImage(i)->As<GpuImageVk<Target>>()->_SetVkImage(VK_NULL_HANDLE);
	}

	vkDestroySwapchainKHR(
		gpu.As<GpuVk<Target>>()->GetLogicalDevice(),
		m_swapchain, nullptr);

	CreationLogic(*gpu.As<GpuVk<Target>>(), newResolution, m_queueIndices.GetFullSpan());
}

template <VulkanTarget Target>
VkColorSpaceKHR SwapchainVk<Target>::GetSupportedColorSpace(const GpuVk<Target>& device) {
	for (const auto& format : device.GetInfo().swapchainSupportDetails.formats)
		if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_SRGB)
			return format.colorSpace;

	return device.GetInfo().swapchainSupportDetails.formats[0].colorSpace;
}

template <VulkanTarget Target>
void SwapchainVk<Target>::Present() {
	// Not needed in Vulkan.
}

#endif
