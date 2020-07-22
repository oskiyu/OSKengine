#pragma once

#include <vulkan/vulkan.h>

#include "RenderMode.h"
#include "PresentMode.h"
#include <string>
#include "WindowAPI.h"
#include "Version.h"
#include "GPUinfo.h"
#include "VulkanImage.h"
#include "VulkanBuffer.h"
#include "Texture.h"
#include "SpriteBatch.h"
#include "Camera2D.h"
#include <unordered_map>
#include "Font.h"

namespace OSK {

	class OSKAPI_CALL VulkanRenderer {

	public:
		
		//Inicializa el renderizador.
		void Init(const RenderMode& mode, const std::string& appName, const Version& gameVersion);

		//Renderiza el frame.
		void RenderFrame();

		//Renderiza la interfaz de usuario de la ventana asociada.
		void DrawUserInterface(SpriteBatch& spriteBatch) const;


		//Establece el modo de presentación deseado.
		//Recrea el swapchain si es necesario.
		void SetPresentMode(const PresentMode& mode);

		//Obtiene el modo de presentación actual.
		PresentMode GetCurrentPresentMode() const;

		//Recarga los shaders.
		void ReloadShaders();

		//Cierra el renderizador.		
		void Close();

		//Carga una textura.
		Texture* LoadTexture(const std::string& path);

		//Carga un sprite.
		void LoadSprite(Sprite* texture, const std::string& path);

		//Establece el spriteBatch que se va a renderizar al llamar a RenderFrame().
		void SubmitSpriteBatch(const SpriteBatch& spriteBatch);

		//Actualiza el UBO.
		void UpdateDefaultUBO(void* ubo);


		void RecreateSwapchain();

		//Carga una fuente.
		void LoadFont(Font& fuente, const std::string& source, uint32_t size);

		//Crea un spriteBatch.
		SpriteBatch CreateSpriteBatch();


		struct {
			std::string VertexShaderPath2D = "Shaders/2D/vert.spv";
			std::string FragmentShaderPath2D = "Shaders/2D/frag.spv";

			std::string VertexShaderPath3D = "Shaders/vert.spv";
			std::string FragmentShaderPath3D = "Shaders/frag.spv";

			//size_t DefaultPushConstantsSize;
			//size_t DefaultUBOSize;

			uint32_t MaxTextures = 32;

			bool AutoUpdateCommandBuffers = true;
		} Settings;


		std::vector<VkCommandBuffer> CommandBuffers;

		//Camera bidimensional.
		Camera2D DefaultCamera2D;

		//Ventana asociada.
		WindowAPI* Window = nullptr;

		//Límite de FPS.
		float FPSlimit = INFINITE;

	private:

		//Crea la instancia de Vulkan.
		void createInstance(const std::string& appName, const Version& gameVersion);

		//Establece la consola para las capas de validación.
		void setupDebugConsole();

		//Crea la suprficie de renerizado.
		void createSurface();

		//Obtiene la GPU que se va a usar.
		void getGPU();

		void createLogicalDevice();

		void createSwapchain();

		void createSwapchainImageViews();

		void createRenderpass();

		void createDescriptorSetLayout();

		void createGraphicsPipeline2D();

		void createGraphicsPipeline3D();

		void createCommandPool();

		void createDepthResources();

		void createFramebuffers();

		void createGlobalImageSampler();

		void createDefaultUniformBuffers();

		void createDescriptorPool();

		void createSyncObjects();

		void createCommandBuffers();

		void createImage(VULKAN::VulkanImage* image, const uint32_t& width, const uint32_t& height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

		void createImageView(VULKAN::VulkanImage* img, VkFormat format, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT) const;

		void destroyImage(VULKAN::VulkanImage* img) const;

		void closeSwapchain();

		void createBuffer(VULKAN::VulkanBuffer* buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags prop) const;

		void destroyBuffer(VULKAN::VulkanBuffer* buffer) const;

		void copyBufferToImage(VULKAN::VulkanBuffer* buffer, VULKAN::VulkanImage* img, const uint32_t& width, const uint32_t& height) const;

		void transitionImageLayout(VULKAN::VulkanImage* img, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const;

		void updateCommandBuffers();

		void createVertexBuffer(VULKAN::VulkanRenderizableObject* obj) const;

		void createIndexBuffer(VULKAN::VulkanRenderizableObject* obj) const;

		void createSpriteVertexBuffer(Sprite* obj) const;

		void createSpriteIndexBuffer(Sprite* obj) const;

		void updateSpriteVertexBuffer(Sprite* obj) const;

		void destroyTexture(Texture* texture) const;

		void destroyAllTextures();

		void destroyAllSprites();

		VULKAN::VulkanImage createImageFromBitMap(uint32_t width, uint32_t height, uint8_t* pixels);

		//Copia el contenido de un buffer a otro buffer.
		void copyBuffer(VULKAN::VulkanBuffer source, VULKAN::VulkanBuffer destination, VkDeviceSize size, VkDeviceSize sourceOffset = 0, VkDeviceSize destinationOffset = 0) const;

		VkCommandBuffer beginSingleTimeCommandBuffer() const;

		void endSingleTimeCommandBuffer(VkCommandBuffer cmdBuffer) const;

		//Obtiene el tipo de memoria indicado.
		uint32_t getMemoryType(const uint32_t& memoryTypeFilter, VkMemoryPropertyFlags flags) const;

		VkShaderModule getShaderModule(const std::vector<char>& code) const;

		VULKAN::SwapchainSupportDetails getSwapchainSupportDetails(VkPhysicalDevice gpu) const;

		//Comprueba si una GPU es compatible.
		VULKAN::GPUinfo getGPUinfo(VkPhysicalDevice gpu) const;

		VULKAN::QueueFamilyIndices findQueueFamilies(VkPhysicalDevice gpu) const;

		VkFormat getSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

		VkFormat getDepthFormat() const;

		void createDescriptorSets(Texture* texture) const;

		//Obtiene el modo de presentación:
		//Cómo se cambian las imágenes del swapchain.
		//
		//	VK_PRESENT_MODE_IMMEDIATE_KHR: se presentan directamente (tearing).
		//	VK_PRESENT_MODE_FIFO_KHR: VSync.
		//	VK_PRESENT_MODE_FIFO_RELAXED_KHR: VSync, tearing.
		//	VK_PRESENT_MODE_MAILBOX_KHR: "triple buffer".
		VkPresentModeKHR getPresentMode(const std::vector<VkPresentModeKHR>& modes) const;

		VkExtent2D getSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

		VULKAN::VulkanImage DepthImage;

		VkDescriptorPool DescriptorPool;

		//std::vector<VULKAN::VulkanBuffer> defaultUniformBuffers;

		std::vector<VULKAN::VulkanBuffer> UniformBuffers2D;

		//Debug messages.
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

		//Carga la función necesaria para establecer la consola de capas de validación.
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

		//Modo de renderizado del motor.
		RenderMode renderMode;

		//Instancia de Vulkan.
		VkInstance Instance;

		//Swapchain.
		VkSwapchainKHR Swapchain;

		//Imágenes del swapchain.
		std::vector<VkImage> SwapchainImages;

		//ImageViews del swapchain (cómo acceder a las imágenes).
		std::vector<VkImageView> SwapchainImageViews;

		//Formato del swapchain.
		VkFormat SwapchainFormat;

		//Tamaño del swapchain.
		VkExtent2D SwapchainExtent;

		//Modo de presentación.
		PresentMode targetPresentMode = PresentMode::VSYNC;

		//GPU.
		VkPhysicalDevice GPU;

		//Superficie sobre la cual se renderiza las imágenes.
		VkSurfaceKHR Surface;

		//Logical device.
		VkDevice LogicalDevice;

		std::vector<VkFramebuffer> Framebuffers;

		//Sync.
		//GPU to CPU.
		std::vector<VkSemaphore> ImageAvailableSemaphores;
		std::vector<VkSemaphore> RenderFinishedSemaphores;

		//CPU to GPU.
		std::vector<VkFence> InFlightFences;
		std::vector<VkFence> ImagesInFlight;

		//Colas.
		VkQueue GraphicsQ;
		VkQueue PresentQ;

		//Renderpass.
		VkRenderPass Renderpass;

		VkPipeline GraphicsPipeline2D;
		VkPipeline GraphicsPipeline3D;

		VkPipelineLayout PipelineLayout2D;
		VkPipelineLayout PipelineLayout3D;

		VkCommandPool CommandPool;

		VkSampler GlobalImageSampler;

		VkDescriptorSetLayout DescriptorSetLayout;

		//Nos permite imprimir mensajes de las capas de validación.
		VkDebugUtilsMessengerEXT debugConsole;

		VULKAN::VulkanRenderizableObject sprite;

		struct {
			bool hasFramebufferBeenResized = false;
			uint32_t currentImage = 0;
		} renderVars;

		SpriteBatch currentSpriteBatch{};
		
		std::unordered_map<std::string, Texture*> textureFromString = {};
		std::vector<Texture> textures = {};

		std::vector<Sprite*> sprites = {};

		bool hasBeenInit = false;
	};

}