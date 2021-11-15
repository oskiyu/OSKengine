#include "SwapchainVulkan.h"

#include <vulkan/vulkan.h>
#include "GpuVulkan.h"
#include "FormatVulkan.h"
#include "Window.h"
#include "Assert.h"

using namespace OSK;

void SwapchainVulkan::Create(Format format, const GpuVulkan& device, const Window& window) {
	auto info = device.GetInfo();
	
	//Formato del swapchain.
	VkSurfaceFormatKHR surfaceFormat;
	surfaceFormat.colorSpace = GetSupportedColorSpace(device);
	surfaceFormat.format = GetFormatVulkan(format);

	//Modo de pressentación.
	VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;

	//tamaño.
	VkExtent2D extent;
	extent.width = window.GetWindowSize().X;
	extent.height = window.GetWindowSize().Y;

	//Número de imágenes en el swapchain.
	imageCount = info.swapchainSupportDetails.surfaceCapabilities.minImageCount + 1;
	//Asegurarnos de que no hay más de las soportadas.
	if (info.swapchainSupportDetails.surfaceCapabilities.maxImageCount > 0 && imageCount > info.swapchainSupportDetails.surfaceCapabilities.maxImageCount)
		imageCount = info.swapchainSupportDetails.surfaceCapabilities.maxImageCount;

	//Create info.
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = device.GetSurface();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	//Colas.
	//Cómo se maneja el swapchain.
	const QueueFamilyIndices indices = device.GetQueueFamilyIndices(device.GetSurface());
	const uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	createInfo.preTransform = info.swapchainSupportDetails.surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //¿Debería mostrarse lo que hay detrás de la ventana?
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE; //Si hay algo tapando la ventana, no se renderiza.
	createInfo.oldSwapchain = nullptr;

	//Crearlo y error-handling.
	VkResult result = vkCreateSwapchainKHR(device.GetLogicalDevice(), &createInfo, nullptr, &swapchain);
	OSK_ASSERT(result == VK_SUCCESS, "No se ha podido crear el swapchain. Code: " + std::to_string(result));
}

VkColorSpaceKHR SwapchainVulkan::GetSupportedColorSpace(const GpuVulkan& device) {
	for (auto format : device.GetInfo().swapchainSupportDetails.formats)
		if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_SRGB)
			return format.colorSpace;

	return device.GetInfo().swapchainSupportDetails.formats[0].colorSpace;
}
