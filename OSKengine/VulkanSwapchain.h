#pragma once

#include <stdlib.h>
#include <string>
#include <vector>
#include <array>

#include <vulkan/vulkan.h>

#include "VulkanTools.h"
#include "Log.h"

#include "WindowAPI.h"

#include "QueueFamilyIndices.h"

#include "VulkanImage.h"
#include "VulkanCommandBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanUniformBufferObject.h"
#include "Camera3D.h"

namespace OSK::VULKAN {

	struct VulkanRenderpass;

	struct VulkanDevice;

	struct VulkanSurface;

	struct VulkanModel;

	struct VulkanCommandBuffer;

	struct VulkanSwapchain {

	public:

		const int32_t MAX_FRAMES_IN_FLIGHT = 2;


		VkSwapchainKHR Swapchain;


		std::vector<VkImage> SwapchainImages;


		std::vector<VkImageView> SwapchainImageViews;


		std::vector<VkFramebuffer> SwapchainFramebuffers;


		std::vector<VulkanBuffer> UniformBuffers;


		VkFormat SwapchainImageFormat;


		VkExtent2D SwapchainExtent;


		VkImageView DepthImageView;


		VulkanImage DepthImage;


		VkDescriptorPool DescriptorPool;


		uint32_t MipLevels = 4;


		size_t CurrentFrame = 0;


		Camera3D* Camera = nullptr;


		VulkanUBO UBO;


		std::vector<VulkanCommandBuffer> CommandBuffers;


		std::vector<VulkanModel*> ModelsToDraw;


		//Sync objects
		std::vector<VkSemaphore> ImageAvaiableSemaphores;


		std::vector<VkSemaphore> RenderFinishedSemaphores;


		std::vector<VkFence> InFlightFences;


		std::vector<VkFence> ImagesInFlight;


		bool SwapchainMustBeRecreated = false;


		bool HasFramebufferBeenResized = false;


		VkDescriptorSetLayout DescriptorSetLayout;


		void CreateDescriptorSetLayout(const VulkanDevice& device);


		void CreateSyncObjects(const VulkanDevice& device);


		//
		void CreateDescriptorPool(const VulkanDevice& device);


		void CreateSwapchain(const VulkanDevice& device, const VulkanSurface& surface, const OSK::WindowAPI& window);


		void CreateFramebuffers(const VulkanDevice& device, const VulkanRenderpass& renderpass);


		void Render(const VulkanDevice& device, const VulkanRenderpass renderpass, const VulkanCommandPool& commandPool, VkPipeline graphicsPipeline, VkPipelineLayout layout);


		void FinishRender(const VulkanDevice& device, VkQueue surfaceQ, VkQueue graphicsQ);


		void Destroy(const VulkanDevice& device);


		//Crea las imágenes con las que trabajará el swapchain
		void CreateImageViews(const VulkanDevice& device);


		void CreateUniformBuffers(const VulkanDevice& device);


		void RecreateSwapchain(const VulkanDevice& device, const VulkanSurface& surface, const OSK::WindowAPI& windowAPI);


		void DrawModel(VulkanModel* model);


		void UpdateUBO(const VulkanModel& model, const VulkanDevice& device);


		SwapChainSupportDetails QueueSwapchainSupport(VkPhysicalDevice device, VulkanSurface surface);


		//Elige el formato de swapchain a a suar de los formatos disponible
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);


		//Elige el modo de presentación a usar
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR >& availablePresentModes);


		//El extent es la resolución de la imagen del swapchain
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const OSK::WindowAPI& window);


		//Devuelve la información sobre las capacidades de swapchain de la gráfica
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VulkanSurface surface);


		VkFormat FindDepthFormat(const VulkanDevice& device);


		VkFormat FindSupportedFormat(const VulkanDevice& device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	};

}