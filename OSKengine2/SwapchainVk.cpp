#include "SwapchainVk.h"

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

SwapchainVk::~SwapchainVk() {
	const VkDevice device = Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice();

	vkDestroySwapchainKHR(device, swapchain, nullptr);

	for (auto& i : images) {
		GpuImageVk* image = i->As<GpuImageVk>();

		vkDestroyImageView(device, image->GetSwapchainView(), nullptr);
		image->_SetVkImage(VK_NULL_HANDLE);
	}
}

void SwapchainVk::Create(PresentMode mode, Format format, const GpuVk& device, const IO::IDisplay& display) {
	this->display = &display;
	this->device = &device;
	this->colorFormat = format;

	this->mode = mode;

	auto& info = device.GetInfo();
	
	//Formato del swapchain.
	VkSurfaceFormatKHR surfaceFormat{};
	surfaceFormat.colorSpace = GetSupportedColorSpace(device);
	surfaceFormat.format = GetFormatVk(format);

	//Modo de pressentación.
	VkPresentModeKHR presentMode = GetPresentModeVk(mode);

	//tamaño.
	VkExtent2D extent{};
	extent.width = display.GetResolution().x;
	extent.height = display.GetResolution().y;

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

	// Colas.
	// Cómo se maneja el swapchain.
	const auto graphicsIndex = Engine::GetRenderer()->GetGraphicsCommandQueue()->As<CommandQueueVk>()->GetFamilyIndex();
	const auto presentIndex = Engine::GetRenderer()->GetPresentCommandQueue()->As<CommandQueueVk>()->GetFamilyIndex();
	DynamicArray<uint32_t> indices = { graphicsIndex , presentIndex };

	if (graphicsIndex != presentIndex) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = indices.GetData();
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	createInfo.preTransform = info.swapchainSupportDetails.surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // ¿Debería mostrarse lo que hay detrás de la ventana?
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE; // Si hay algo tapando la ventana, no se renderiza.
	createInfo.oldSwapchain = nullptr;

	// Crearlo y error-handling.
	VkResult result = vkCreateSwapchainKHR(device.GetLogicalDevice(), &createInfo, nullptr, &swapchain);
	OSK_ASSERT(result == VK_SUCCESS, SwapchainCreationException("No se ha podido crear el swapchain", result));

	for (UIndex32 i = 0; i < imageCount; i++)
		images[i] = new GpuImageVk({ extent.width, extent.height, 1 }, GpuImageDimension::d2D, GpuImageUsage::COLOR, 1, format, 1, {});

	AcquireImages(extent.width, extent.height);
	AcquireViews();
}

void SwapchainVk::AcquireImages(unsigned int sizeX, unsigned int sizeY) {
	VkResult result = vkGetSwapchainImagesKHR(device->GetLogicalDevice(), swapchain, &imageCount, nullptr);
	OSK_ASSERT(result == VK_SUCCESS, SwapchainCreationException("Error al adquirir imagenes del swapchain", result));

	VkImage* tempImages = new VkImage[imageCount];
	vkGetSwapchainImagesKHR(device->GetLogicalDevice(), swapchain, &imageCount, tempImages);
	OSK_ASSERT(result == VK_SUCCESS, SwapchainCreationException("Error al adquirir imagenes del swapchain", result));

	for (UIndex32 i = 0; i < imageCount; i++)
		images[i]->As<GpuImageVk>()->_SetVkImage(tempImages[i]);

	delete[] tempImages;
}

void SwapchainVk::AcquireViews() {
	auto tempViews = new VkImageView[imageCount];

	for (UIndex32 i = 0; i < imageCount; i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = images[i]->As<GpuImageVk>()->GetVkImage();
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = GetFormatVk(colorFormat);
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
		OSK_ASSERT(result == VK_SUCCESS, SwapchainCreationException("Error al crear view de imagen del swapchain", result));

		if (images[i]->As<GpuImageVk>()->GetSwapchainView() != VK_NULL_HANDLE)
			vkDestroyImageView(Engine::GetRenderer()->GetGpu()->As<GpuVk>()->GetLogicalDevice(), images[i]->As<GpuImageVk>()->GetSwapchainView(), 0);
		images[i]->As<GpuImageVk>()->SetSwapchainView(tempViews[i]);
	}

	delete[] tempViews;
}

VkSwapchainKHR SwapchainVk::GetSwapchain() const {
	return swapchain;
}

void SwapchainVk::Resize() {
	for (auto& i : images) {
		i->As<GpuImageVk>()->_SetVkImage(VK_NULL_HANDLE);
	}

	vkDestroySwapchainKHR(Engine::GetRenderer()->As<RendererVk>()->GetGpu()->As<GpuVk>()->GetLogicalDevice(),
		swapchain, nullptr);

	Create(mode, colorFormat, *device, *display);
}

VkColorSpaceKHR SwapchainVk::GetSupportedColorSpace(const GpuVk& device) {
	for (const auto& format : device.GetInfo().swapchainSupportDetails.formats)
		if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_SRGB)
			return format.colorSpace;

	return device.GetInfo().swapchainSupportDetails.formats[0].colorSpace;
}

void SwapchainVk::Present() {

}
