#include "VulkanSwapchain.h"

#include "VulkanRenderpass.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include "VulkanModel.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanUniformBufferObject.h"

namespace OSK::VULKAN {

	void VulkanSwapchain::CreateDescriptorSetLayout(const VulkanDevice& device) {
		//Shaders
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = NULL;

		//Texturas
		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		samplerLayoutBinding.pImmutableSamplers = NULL;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		VkResult result = vkCreateDescriptorSetLayout(device.LogicalDevice, &layoutInfo, NULL, &DescriptorSetLayout);

		if (result != VK_SUCCESS) {
			OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "crear descriptor layout", __LINE__);
			throw std::runtime_error("ERROR: crear descriptor layout");
		}
	}


	void VulkanSwapchain::CreateSyncObjects(const VulkanDevice& device) {
		//Inicializar los std::vector que almacenan los semáforos
		ImageAvaiableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		ImagesInFlight.resize(SwapchainImages.size(), VK_NULL_HANDLE);

		//struct con la información de los semáforos
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO; //Tipo de struct

		//struct con la información de las fences
		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO; //Tipo de struct
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		//
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

			//Crear los semáforos y error handling
			VkResult result = vkCreateSemaphore(device.LogicalDevice, &semaphoreInfo, NULL, &ImageAvaiableSemaphores[i]);
			if (result != VK_SUCCESS) {
				throw std::runtime_error("ERROR: crear semáforo");
			}

			result = vkCreateSemaphore(device.LogicalDevice, &semaphoreInfo, NULL, &RenderFinishedSemaphores[i]);
			if (result != VK_SUCCESS) {
				throw std::runtime_error("ERROR: crear semáforo");
			}

			//Crear las fences
			result = vkCreateFence(device.LogicalDevice, &fenceInfo, NULL, &InFlightFences[i]);
			if (result != VK_SUCCESS) {
				throw std::runtime_error("ERROR: crear vallas");
			}

		}
	}


	void VulkanSwapchain::CreateDescriptorPool(const VulkanDevice& device) {
		std::array<VkDescriptorPoolSize, 2> poolSizes = {};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(SwapchainImages.size());

		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(SwapchainImages.size());

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(SwapchainImages.size());


		VkResult result = vkCreateDescriptorPool(device.LogicalDevice, &poolInfo, NULL, &DescriptorPool);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("ERROR: crear descriptor pool");
		}
	}


	void VulkanSwapchain::CreateSwapchain(const VulkanDevice& device, const VulkanSurface& surface, const OSK::WindowAPI& window) {
		//struct con la información del soporte del swapchain
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device.PhysicalDevice, surface);

		//Diferentes aspectos del swapchain
		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats); //Formato a usar
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes); //Modo de presentación a usar
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, window); //Resolución del swapchain

		//Cantidad de imágenes en el swapchain
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1; //+1 para asegurarnos de que no tendremos que esperar a la GPU para que deje una imagen libre para seguir renderizando
		
		//Comprobar que el swapchain no tendrá más imágenes de las soportadas
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) { //'swapChainSupport.capabilities.maxImageCount = 0' si no hay un límite establecido
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		//Struct con la información del swapchain
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR; //Tipo de struct
		createInfo.surface = surface.Surface; //Superficie que va a usar
		createInfo.minImageCount = imageCount; //Número de imágenes que va a almacenar
		createInfo.imageFormat = surfaceFormat.format; //Formato a usar
		createInfo.imageColorSpace = surfaceFormat.colorSpace; //Espacio de color a usar
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		//
		QueueFamilyIndices indices = device.FindQueueFamilies(surface, device.PhysicalDevice);
		uint32_t queueFamilyIndices[] = {
			indices.GraphicsFamily.value(),
			indices.PresentFamily.value()
		};

		//Comprobar si las familias de comando de gráficos y presentación son la misma o no
		if (indices.GraphicsFamily != indices.PresentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		//Transformación de las imágenes en el swapchain
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform; //Ninguna

		//Si las partes transparentes de la ventana deberían mostrar lo que hay detras (escritorio, etc.)
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //No

		//No calcular los píxeles que están tapados por otra(s) ventana(s)
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		//TODO
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		//Crear el swapchain
		VkResult result = vkCreateSwapchainKHR(device.LogicalDevice, &createInfo, NULL, &Swapchain);

		if (result != VK_SUCCESS) {
			OSK::Logger::Log(OSK::LogMessageLevels::BAD_ERROR, "ERROR: swap chain", __LINE__);
			throw std::runtime_error("ERROR: swap chain");
		}

		//Almacena los handles a las imágenes del swapchain
		vkGetSwapchainImagesKHR(device.LogicalDevice, Swapchain, &imageCount, NULL);
		SwapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device.LogicalDevice, Swapchain, &imageCount, SwapchainImages.data());

		//Almacena otras características del swapchain
		SwapchainImageFormat = surfaceFormat.format;
		SwapchainExtent = extent;

		OSK::Logger::Log(OSK::LogMessageLevels::INFO, "INFO: swap chain created", __LINE__);

		CreateImageViews(device);

		CreateDescriptorPool(device);
		CreateDescriptorSetLayout(device);
	}


	void VulkanSwapchain::CreateFramebuffers(const VulkanDevice& device, const VulkanRenderpass& renderpass) {
		VkFormat depthFormat = FindDepthFormat(device);
		DepthImage.CreateImage(device, SwapchainExtent.width, SwapchainExtent.height, MipLevels, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		DepthImageView = VulkanImage::CreateImageView(device, DepthImage.Image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, MipLevels);

		//Inicializar el std::vector
		SwapchainFramebuffers.resize(SwapchainImages.size());

		//Crear un framebuffer por cada imagen
		for (size_t i = 0; i < SwapchainImageViews.size(); i++) {

			//La imagen con la que se va a enlazar el framebuffer
			std::array<VkImageView, 2> attachments = {
				SwapchainImageViews[i],
				DepthImageView
			};

			//struct con la información del framebuffer
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO; //Tipo de struct
			framebufferInfo.renderPass = renderpass.RenderPass; //Render pass
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = SwapchainExtent.width; //Ancho
			framebufferInfo.height = SwapchainExtent.height; //Alto
			framebufferInfo.layers = 1;

			//Crear el framebuffer
			VkResult result = vkCreateFramebuffer(device.LogicalDevice, &framebufferInfo, NULL, &SwapchainFramebuffers[i]);

			//Error handling
			if (result != VK_SUCCESS) {
				OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "crear framebuffer: " + i, __LINE__);
				throw std::runtime_error("ERROR: crear framebuffer");
			}
		}
	}


	void VulkanSwapchain::Render(const VulkanDevice& device, const VulkanRenderpass renderpass, const VulkanCommandPool& commandPool, VkPipeline graphicsPipeline, VkPipelineLayout layout) {
		
		UBO.view = Camera->GetView();
		UBO.projection = Camera->GetProjection();
		
		//Inicializar el std::vector que los almacena
		CommandBuffers.resize(SwapchainImages.size());

		//Struct con la información de los command buffers
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO; //Tipo de struct
		allocInfo.commandPool = commandPool.CommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)CommandBuffers.size();

		//Crear el command buffer
		std::vector<VkCommandBuffer> commandBuffers;
		commandBuffers.resize(CommandBuffers.size());
		for (uint32_t i = 0; i < CommandBuffers.size(); i++) {
			commandBuffers[i] = CommandBuffers[i].CommandBuffer;
		}

		VkResult result = vkAllocateCommandBuffers(device.LogicalDevice, &allocInfo, commandBuffers.data());

		//Error handling
		if (result != VK_SUCCESS) {
			throw std::runtime_error("ERROR: crear command buffers");
		}

		for (uint32_t i = 0; i < CommandBuffers.size(); i++) {
			CommandBuffers[i].CommandBuffer = commandBuffers[i];
		}

		//Comandos de inicialización en los command buffer
		for (size_t i = 0; i < CommandBuffers.size(); i++) {

			//Struct con la inforrmación del comando de inicialización
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO; //Tipo de struct
			beginInfo.flags = 0; // Optional
			beginInfo.pInheritanceInfo = NULL; // Optional

			//Crear comando
			result = vkBeginCommandBuffer(CommandBuffers[i].CommandBuffer, &beginInfo);

			//Error handling
			if (result != VK_SUCCESS) {
				throw std::runtime_error("ERROR: comando de inicialización");
			}

			std::array<VkClearValue, 2> clearValues = {};
			clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };

			//Iniciar el render pass en cada command buffer
			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO; //Tipo de struct
			renderPassInfo.renderPass = renderpass.RenderPass; //Render pass
			renderPassInfo.framebuffer = SwapchainFramebuffers[i]; //Framebuffer
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = SwapchainExtent;
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			//Iniciar el render pass
			vkCmdBeginRenderPass(CommandBuffers[i].CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			//Enlazar con el graphics pipeline
			vkCmdBindPipeline(CommandBuffers[i].CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

			for (auto model : ModelsToDraw) {
				UpdateUBO(*model, device);
				model->Draw(CommandBuffers[i], layout, i);
			}

			//Fin del render pass
			vkCmdEndRenderPass(CommandBuffers[i].CommandBuffer);

			//Fin del command buffer
			result = vkEndCommandBuffer(CommandBuffers[i].CommandBuffer);

			//Error handling
			if (result != VK_SUCCESS) {
				throw std::runtime_error("ERROR: finalizar command buffer");
			}
		}
	}


	void VulkanSwapchain::FinishRender(const VulkanDevice& device, VkQueue surfaceQ, VkQueue graphicsQ) {
		//Sync
		vkWaitForFences(device.LogicalDevice, 1, &InFlightFences[CurrentFrame], VK_TRUE, UINT64_MAX);
		uint32_t imageIndex;
		//Adquirir la imagen sobre la que vamos a renderizar
		VkResult result = vkAcquireNextImageKHR(device.LogicalDevice, Swapchain, UINT64_MAX, ImageAvaiableSemaphores[CurrentFrame], VK_NULL_HANDLE, &imageIndex);

		//Error handling y otros handling
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			SwapchainMustBeRecreated = true;
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("ERROR: Render()");
		}

		if (ImagesInFlight[imageIndex] != VK_NULL_HANDLE) {
			vkWaitForFences(device.LogicalDevice, 1, &ImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
		}
		ImagesInFlight[imageIndex] = InFlightFences[CurrentFrame];

		//Añadir comando
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO; //Tipo de struct

		VkSemaphore waitSemaphores[] = { ImageAvaiableSemaphores[CurrentFrame] };

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &CommandBuffers[imageIndex].CommandBuffer;

		VkSemaphore signalSemaphores[] = { RenderFinishedSemaphores[CurrentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(device.LogicalDevice, 1, &InFlightFences[CurrentFrame]);
		
		//Error handling
		result = vkQueueSubmit(graphicsQ, 1, &submitInfo, InFlightFences[CurrentFrame]);

		if (result != VK_SUCCESS) {
			OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "draw command buffer", __LINE__);
			throw std::runtime_error("ERROR: draw command buffer");
		}

		//Enviar la imagen al swapchan
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR; //Tipo de struct
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { Swapchain }; //Swapchain a usar
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = NULL; // Optional

		//Fin
		Logger::DebugLog(std::to_string(imageIndex));
		result = vkQueuePresentKHR(surfaceQ, &presentInfo);//BUG

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || HasFramebufferBeenResized) {
			SwapchainMustBeRecreated = true;
			HasFramebufferBeenResized = false;
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("ERROR: vkQueuePresentKHR");
		}

		//Esperar sincronización
		vkQueueWaitIdle(surfaceQ);

		CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}


	void VulkanSwapchain::Destroy(const VulkanDevice& device) {
		//Destruye los framebuffers
		for (size_t i = 0; i < SwapchainFramebuffers.size(); i++) {
			vkDestroyFramebuffer(device.LogicalDevice, SwapchainFramebuffers[i], NULL);
		}

		for (auto imageView : SwapchainImageViews) {
			vkDestroyImageView(device.LogicalDevice, imageView, nullptr);
		}

		for (auto i : UniformBuffers) {
			i.DestroyBuffer(device);
		}

		DepthImage.Destroy(device);

		vkDestroyDescriptorSetLayout(device.LogicalDevice, DescriptorSetLayout, nullptr);

		vkDestroyImageView(device.LogicalDevice, DepthImageView, nullptr);

		vkDestroySwapchainKHR(device.LogicalDevice, Swapchain, nullptr);
	}


	void VulkanSwapchain::CreateImageViews(const VulkanDevice& device) {
		//Cambiar el tamaño del std::vector al número de imágenes del swapchain
		SwapchainImageViews.resize(SwapchainImages.size());

		//Por cada imagen...
		for (size_t i = 0; i < SwapchainImages.size(); i++) {
			SwapchainImageViews[i] = VulkanImage::CreateImageView(device, SwapchainImages[i], SwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, MipLevels);
		}
	}


	void VulkanSwapchain::CreateUniformBuffers(const VulkanDevice& device) {
		VkDeviceSize bufferSize = sizeof(VulkanUBO);

		UniformBuffers.resize(SwapchainImages.size());

		for (size_t i = 0; i < SwapchainImages.size(); i++) {
			UniformBuffers[i].CreateBuffer(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
	}


	void VulkanSwapchain::RecreateSwapchain(const VulkanDevice& device, const VulkanSurface& surface, const OSK::WindowAPI& windowAPI) {
		Destroy(device);
		CreateSwapchain(device, surface, windowAPI);
	}


	void VulkanSwapchain::DrawModel(VulkanModel* model) {
		ModelsToDraw.push_back(model);
	}


	void VulkanSwapchain::UpdateUBO(const VulkanModel& model, const VulkanDevice& device) {
		UBO.model = model.ModelTransform.ModelMatrix;
		
		void* data;
		vkMapMemory(device.LogicalDevice, UniformBuffers[CurrentFrame].BufferMemory, 0, sizeof(VulkanUBO), 0, &data);
		memcpy(data, &UBO, sizeof(VulkanUBO));
		vkUnmapMemory(device.LogicalDevice, UniformBuffers[CurrentFrame].BufferMemory);
	}


	SwapChainSupportDetails VulkanSwapchain::QueueSwapchainSupport(VkPhysicalDevice device, VulkanSurface surface) {
		//Struct que almacena la información sobre las capacidades de la GPU para efectuar el swapChain
		SwapChainSupportDetails details;

		//Obtiene las capacidades de la GPU para efectuar el swapChain
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface.Surface, &details.capabilities);

		//Comprobar que formatos de swapChain están soortados por la GPU
		uint32_t formatCount = 0; //Número de formatos soportados
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.Surface, &formatCount, NULL); //Obtiene la información sobre los formatos soportados

		//Si hay al menos un formato soportado, se añade(n) al struct que almacena esta información
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.Surface, &formatCount, details.formats.data());
		}

		//Comprobar que modos de presentación están soortados por la GPU
		uint32_t presentModeCount; //Número de modos soportados
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.Surface, &presentModeCount, NULL); //Obtiene la información sobre los modos soportados

		//Si hay al menos un modo soportado, se añade(n) al struct que almacena esta información
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.Surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}


	VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

		//Comrpueba si están soportadas los mejores espacios de color
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) { //RGB ; SRGB
				return availableFormat;
			}
		}

		//Si no está disponible el formato óptimo, continuar con el defecto
		return availableFormats[0];
	}


	//Elige el modo de presentación a usar
	VkPresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR >& availablePresentModes) {

		/* Modos:

			'VK_PRESENT_MODE_IMMEDIATE_KHR' = sin V_Sync

			'VK_PRESENT_MODE_FIFO_KHR' = doble buffer

			'VK_PRESENT_MODE_MAILBOX_KHR' = triple buffer
		*/

		//Primero comprobar que el triple buffer está soportado (y usarlo en este caso)
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		//Si no, tratar de usar el doble buffer
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR) {
				return availablePresentMode;
			}
		}

		//Si ninguno de los dos está soportado, no usar V_Sync
		return VK_PRESENT_MODE_IMMEDIATE_KHR;
	}


	//El extent es la resolución de la imagen del swapchain
	VkExtent2D VulkanSwapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const OSK::WindowAPI& window) {

		//
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		}
		else {

			int width;
			int height;

			glfwGetFramebufferSize(window.GetGLFWWindow(), &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(window.ScreenSizeX),
				static_cast<uint32_t>(window.ScreenSizeY)
			};

			//Asegurarnos de que la imagen no es más grande (ni más pequeña) de lo soportado
			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}


	//Devuelve la información sobre las capacidades de swapchain de la gráfica
	SwapChainSupportDetails VulkanSwapchain::QuerySwapChainSupport(VkPhysicalDevice device, VulkanSurface surface) {

		//Struct que almacena la información sobre las capacidades de la GPU para efectuar el swapChain
		SwapChainSupportDetails details;

		//Obtiene las capacidades de la GPU para efectuar el swapChain
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface.Surface, &details.capabilities);

		//Comprobar que formatos de swapChain están soortados por la GPU
		uint32_t formatCount = 0; //Número de formatos soportados
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.Surface, &formatCount, NULL); //Obtiene la información sobre los formatos soportados

		//Si hay al menos un formato soportado, se añade(n) al struct que almacena esta información
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.Surface, &formatCount, details.formats.data());
		}

		//Comprobar que modos de presentación están soortados por la GPU
		uint32_t presentModeCount; //Número de modos soportados
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.Surface, &presentModeCount, NULL); //Obtiene la información sobre los modos soportados

		//Si hay al menos un modo soportado, se añade(n) al struct que almacena esta información
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.Surface, &presentModeCount, details.presentModes.data());
		}


		return details;
	}


	VkFormat VulkanSwapchain::FindDepthFormat(const VulkanDevice& device) {
		return FindSupportedFormat(device, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}


	VkFormat VulkanSwapchain::FindSupportedFormat(const VulkanDevice& device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {

		for (VkFormat format : candidates) {
			VkFormatProperties properties;
			vkGetPhysicalDeviceFormatProperties(device.PhysicalDevice, format, &properties);

			if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
				return format;
			}

		}
	}

}