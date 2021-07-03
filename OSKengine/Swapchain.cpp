#include "Swapchain.h"

#include "VulkanRenderer.h"

using namespace OSK;
using namespace OSK::VULKAN;

Swapchain::Swapchain(RenderAPI* renderer) {
	this->renderer = renderer;
}

Swapchain::~Swapchain() {
	Close();
}

void Swapchain::Create() {
	CreateSwapchain();
	AcquireImages();
	CreateImageViews();
}

uint32_t Swapchain::GetImageCount() const {
	return imageCount;
}

VkFormat Swapchain::GetFormat() const {
	return format;
}

void Swapchain::Close() {
	for (auto view : imageViews)
		vkDestroyImageView(renderer->logicalDevice, view, nullptr);
	imageViews.clear();

	if (images)
		delete[] images;
	imageCount = 0;

	vkDestroySwapchainKHR(renderer->logicalDevice, swapchain, nullptr);
}

void Swapchain::CreateSwapchain() {
	renderer->gpuInfo.UpdateSwapchainSupport();

	//Formato del swapchain.
	VkSurfaceFormatKHR surfaceFormat = GetSupportedFormat(renderer->gpuInfo);

	//Modo de pressentación.
	VkPresentModeKHR presentMode = renderer->getPresentMode(renderer->gpuInfo.shapchainSupport.presentModes);

	//tamaño.
	VkExtent2D extent = GetSupportedExtent(renderer->gpuInfo);

	//Número de imágenes en el swapchain.
	uint32_t imageCount = renderer->gpuInfo.shapchainSupport.surfaceCapabilities.minImageCount + 1;

	//Asegurarnos de que no hay más de las soportadas.
	if (renderer->gpuInfo.shapchainSupport.surfaceCapabilities.maxImageCount > 0 && imageCount > renderer->gpuInfo.shapchainSupport.surfaceCapabilities.maxImageCount)
		imageCount = renderer->gpuInfo.shapchainSupport.surfaceCapabilities.maxImageCount;

	//Create info.
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = renderer->surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	//Colas.
	//Cómo se maneja el swapchain.
	const QueueFamilyIndices indices = renderer->findQueueFamilies(renderer->gpuInfo.gpu);
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
	createInfo.preTransform = renderer->gpuInfo.shapchainSupport.surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //¿Debería mostrarse lo que hay detrás de la ventana?
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE; //Si hay algo tapando la ventana, no se renderiza.
	createInfo.oldSwapchain = nullptr;

	//Crearlo y error-handling.
	VkResult result = vkCreateSwapchainKHR(renderer->logicalDevice, &createInfo, nullptr, &swapchain);

	OSK_ASSERT(result == VK_SUCCESS, "Error al crear el swapchain. Code: " + std::to_string(result));

	//Actualizar las variables de la clase.
	format = surfaceFormat.format;
	size = extent;
}

void Swapchain::AcquireImages() {
	VkResult result = vkGetSwapchainImagesKHR(renderer->logicalDevice, swapchain, &imageCount, nullptr);
	OSK_ASSERT(result == VK_SUCCESS, "Error al adquirir imagenes del swapchain. Code: " + std::to_string(result));

	images = new VkImage[imageCount];
	vkGetSwapchainImagesKHR(renderer->logicalDevice, swapchain, &imageCount, images);
	OSK_ASSERT(result == VK_SUCCESS, "Error al adquirir imagenes del swapchain. Code: " + std::to_string(result));
}

void Swapchain::CreateImageViews() {
	imageViews.resize(imageCount);

	for (uint32_t i = 0; i < imageViews.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkResult result = vkCreateImageView(renderer->logicalDevice, &createInfo, nullptr, &imageViews[i]);
		OSK_ASSERT(result == VK_SUCCESS, "Error al crear view de imagen del swapchain. Code: " + std::to_string(result));
	}
}

void Swapchain::RecreateSwapchain() {
	Close();
	Create();
}

VkExtent2D Swapchain::GetSupportedExtent(GpuInfo gpu) const {
	VkSurfaceCapabilitiesKHR capabilities = gpu.shapchainSupport.surfaceCapabilities;

	//Si está inicializado, devolver.
	if (capabilities.currentExtent.width != UINT32_MAX)
		return capabilities.currentExtent;

	int width;
	int height;
	glfwGetFramebufferSize(renderer->window->window, &width, &height);

	VkExtent2D extent{
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height)
	};

	//Que no sea más grande de lo soportado.
	extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, extent.width));
	extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, extent.height));

	return extent;
}

VkSurfaceFormatKHR Swapchain::GetSupportedFormat(GpuInfo gpu) const {
	//Preferir sRGB.
	for (auto format : gpu.shapchainSupport.formats)
		if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_SRGB)
			return format;

	return gpu.shapchainSupport.formats[0];
}
