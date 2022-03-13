#include "SwapchainVulkan.h"

#include <vulkan/vulkan.h>
#include "GpuVulkan.h"
#include "FormatVulkan.h"
#include "Window.h"
#include "GpuImageVulkan.h"
#include "Assert.h"
#include "Logger.h"
#include "OSKengine.h"
#include "RenderpassVulkan.h"
#include "RendererVulkan.h"
#include "GpuVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

SwapchainVulkan::~SwapchainVulkan() {
	vkDestroySwapchainKHR(Engine::GetRenderer()->As<RendererVulkan>()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
		swapchain, nullptr);
}

void SwapchainVulkan::Create(Format format, const GpuVulkan& device, const IO::Window& window) {
	this->window = &window;
	this->device = &device;
	this->format = format;

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

	for (TSize i = 0; i < imageCount; i++)		
		images[i] = new GpuImageVulkan(extent.width, extent.height, format);

	AcquireImages(extent.width, extent.height);
	AcquireViews();

	if (targetRenderpass)
		targetRenderpass->SetImages(images[0].GetPointer(), images[1].GetPointer(), images[2].GetPointer());

	Engine::GetLogger()->InfoLog("Creado correctamente el swapchain.");
}

void SwapchainVulkan::AcquireImages(unsigned int sizeX, unsigned int sizeY) {
	VkResult result = vkGetSwapchainImagesKHR(device->GetLogicalDevice(), swapchain, &imageCount, nullptr);
	OSK_ASSERT(result == VK_SUCCESS, "Error al adquirir imagenes del swapchain. Code: " + std::to_string(result));

	auto tempImages = new VkImage[imageCount];
	vkGetSwapchainImagesKHR(device->GetLogicalDevice(), swapchain, &imageCount, tempImages);
	OSK_ASSERT(result == VK_SUCCESS, "Error al adquirir imagenes del swapchain. Code: " + std::to_string(result));

	for (TSize i = 0; i < imageCount; i++)
		images[i]->As<GpuImageVulkan>()->SetImage(tempImages[i]);

	delete[] tempImages;

	Engine::GetLogger()->InfoLog("	Adquiridas las imágenes del swapchain.");
}

void SwapchainVulkan::AcquireViews() {
	auto tempViews = new VkImageView[imageCount];

	for (TSize i = 0; i < imageCount; i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = images[i]->As<GpuImageVulkan>()->GetImage();
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = GetFormatVulkan(format);
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkResult result = vkCreateImageView(device->GetLogicalDevice(), &createInfo, nullptr, &tempViews[i]);
		OSK_ASSERT(result == VK_SUCCESS, "Error al crear view de imagen del swapchain. Code: " + std::to_string(result));

		if (images[i]->As<GpuImageVulkan>()->GetView() != VK_NULL_HANDLE)
			vkDestroyImageView(Engine::GetRenderer()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(), images[i]->As<GpuImageVulkan>()->GetView(), 0);
		images[i]->As<GpuImageVulkan>()->SetView(tempViews[i]);
	}

	delete[] tempViews;

	Engine::GetLogger()->InfoLog("	Adquiridas las views del swapchain.");
}

VkSwapchainKHR SwapchainVulkan::GetSwapchain() const {
	return swapchain;
}

void SwapchainVulkan::Resize() {
	for (auto& i : images) {
		i->As<GpuImageVulkan>()->SetImage(VK_NULL_HANDLE);
	}

	vkDestroySwapchainKHR(Engine::GetRenderer()->As<RendererVulkan>()->GetGpu()->As<GpuVulkan>()->GetLogicalDevice(),
		swapchain, nullptr);

	Create(format, *device, *window);
}

VkColorSpaceKHR SwapchainVulkan::GetSupportedColorSpace(const GpuVulkan& device) {
	for (auto format : device.GetInfo().swapchainSupportDetails.formats)
		if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_SRGB)
			return format.colorSpace;

	return device.GetInfo().swapchainSupportDetails.formats[0].colorSpace;
}

void SwapchainVulkan::Present() {

}
