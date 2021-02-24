#pragma once

#include <vulkan/vulkan.h>

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
#include "Camera3D.h"
#include <unordered_map>
#include "Font.h"
#include "AnimatedModel.h"
#include "Renderpass.h"
#include "Model.h"
#include "GraphicsPipeline.h"
#include "DescriptorLayout.h"
#include "LightsUBO.h"
#include "Skybox.h"

#include "ContentManager.h"
#include "RenderizableScene.h"

#include "VulkanImageGen.h"
#include "ProfilingUnit.h"
#include "Framebuffer.h"

#include "RenderTarget.h"
#include "RenderStage.h"
#include <functional>

class Game;

namespace OSK {

	class RenderSystem3D;

	class OSKAPI_CALL RenderAPI {

		friend class ContentManager;
		friend class RenderizableScene;
		friend class ShadowMap;
		friend class CubeShadowMap;
		friend class VULKAN::VulkanImageGen;
		friend class VULKAN::Framebuffer;
		friend class Game;
		friend class RenderSystem3D;

	public:
	
		RenderTarget* RTarget = CreateNewRenderTarget();

		//Inicializa el renderizador.
		//	<appName>: nombre del juego.
		//	<gameVersion>: versi�n del juego.
		OskResult Init(const std::string& appName, const Version& gameVersion);

		//Renderiza el frame.
		void RenderFrame();

		//Establece el modo de presentaci�n deseado.
		//Recrea el swapchain si es necesario.
		//	<mode>: modo de presentaci�n objetivo.
		void SetPresentMode(PresentMode mode);

		//Obtiene el modo de presentaci�n actual.
		PresentMode GetCurrentPresentMode() const;

		//Recarga los shaders.
		void ReloadShaders();


		//Cierra el renderizador.		
		void Close();


		//Establece el spriteBatch que se va a renderizar al llamar a RenderFrame().
		//	<spriteBatch>: spriteBatch que se renderizar�.
		void AddSpriteBatch(SpriteBatch* spriteBatch);

		void RemoveSpriteBatch(SpriteBatch* spriteBatch);

		//Recrea el swapchain.
		void RecreateSwapchain();


		//Crea un spriteBatch.
		SpriteBatch CreateSpriteBatch();


		//Crea un nuevo graphics pipeline vac�o.
		//	<vertexPath>: ruta del shader de v�rtices.
		//	<fragmentPath>: ruta del shader de fragmento.
		GraphicsPipeline* CreateNewGraphicsPipeline(const std::string& vertexPath, const std::string& fragmentPath) const;

		//Crea un nuevo descriptor layout vac�o.
		DescriptorLayout* CreateNewDescriptorLayout() const;

		//Crea un nuevo descriptor set vac�o.
		DescriptorSet* CreateNewDescriptorSet() const;
		
		//Phong lighting.
		
		//Crea un nuevo graphics pipeline configurado para el motor de iluminaci�n PHONG.
		//	<vertexPath>: ruta del shader PHONG de v�rtices.
		//	<fragmentPath>: ruta del shader PHONG de fragmento.
		GraphicsPipeline* CreateNewPhongPipeline(const std::string& vertexPath, const std::string& fragmentPath) const;

		//Crea un nuevo descriptor layout configurado para usarse con el motor de iluminaci�n PHONG.
		DescriptorLayout* CreateNewPhongDescriptorLayout(uint32_t maxSets) const;

		//Skybox.
		GraphicsPipeline* CreateNewSkyboxPipeline(const std::string& vertexPath, const std::string& fragmentPath) const;
		DescriptorLayout* CreateNewSkyboxDescriptorLayout() const;
		void CreateNewSkyboxDescriptorSet(SkyboxTexture* texture) const;
		
		//Crea un buffer que almacenar� informaci�n en la GPU.
		//	<buffer>: buffer que se va a crear.
		//	<size>: tama�o del buffer.
		//	<usage>: el uso que se le dar� al buffer.
		//	<prop>: propiedades de memoria que necesitar� el buffer.
		//
		//Para destruir un buffer, llamar a VulkanRenderer::DestroyBuffer.
		void CreateBuffer(VulkanBuffer& buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags prop) const;

		//Destruye un buffer, liberando la memoria que ten�a asignada en la GPU.
		//	<buffer>: buffer a destruir.
		void DestroyBuffer(VulkanBuffer& buffer) const;

		//Copia el contenido de un buffer a otro buffer.
		//	<source>: buffer fuente.
		//	<destination>: buffer al que se copiar� la informaci�n.
		//	<size>: tama�o de la informaci�n que se va a copiar (puede no ser el mismo tama�o que el de los buffers).
		//	<sourceOffset = 0>: offset sobre la informac�on del buffer fuente.
		//	<destinationOffset = 0>: offset sobre el buffer destino.
		void CopyBuffer(VulkanBuffer& source, VulkanBuffer& destination, VkDeviceSize size, VkDeviceSize sourceOffset = 0, VkDeviceSize destinationOffset = 0) const;

		VULKAN::Renderpass* CreateNewRenderpass();

		RenderTarget* CreateNewRenderTarget();
		void InitRenderTarget(RenderTarget* rtarget, ContentManager* content);

		VULKAN::Framebuffer* CreateNewFramebuffer();

		std::vector<VkCommandBuffer> GetCommandBuffers();

		void SetRenderizableScene(RenderizableScene* scene);

		struct {
			std::string VertexShaderPath2D = "Shaders/2D/vert.spv";
			std::string FragmentShaderPath2D = "Shaders/2D/frag.spv";

			std::string VertexShaderPath3D = "Shaders/Vk/vert.spv";
			std::string FragmentShaderPath3D = "Shaders/Vk/frag.spv";

			std::string SkyboxVertexPath = "shaders/VK_Skybox/vert.spv";
			std::string SkyboxFragmentPath = "shaders/VK_Skybox/frag.spv";

			bool AutoUpdateCommandBuffers = true;
		} Settings;


		std::vector<VkCommandBuffer> CommandBuffers;

		//Camaras.
		Camera2D DefaultCamera2D{};
		Camera2D RenderTargetCamera2D{};
		Camera3D DefaultCamera3D{0, 0, 0};

		//Ventana asociada.
		WindowAPI* Window = nullptr;

		//L�mite de FPS.
		float FPSlimit = INFINITE;

		VULKAN::Renderpass* renderpass;

		GraphicsPipeline* DefaultGraphicsPipeline2D;
		GraphicsPipeline* DefaultGraphicsPipeline3D;
		GraphicsPipeline* DefaultSkyboxGraphicsPipeline;

		ContentManager* Content = new ContentManager(this);

		RenderizableScene* Scene;

		ProfilingUnit renderP_Unit{ "Render frame" };
		ProfilingUnit updateCmdP_Unit{ "Update command buffers" };

		Sprite OSKengineIconSprite;
		Sprite OSK_IconSprite;

		unsigned int RenderTargetSizeX = 1024;
		unsigned int RenderTargetSizeY = 720;
		float RenderResolutionMultiplier = 2.0f; //Record = 17.0f { 32640 x 18360 }

		void createDescriptorSets(Texture* texture) const;

		void AddStage(RenderStage* stage);
		void RemoveStage(RenderStage* stage);

		void AddSingleTimeStage(RenderStage* stage);

		VULKAN::VulkanImage DepthImage;

		VkFormat getDepthFormat() const;

		//Formato del swapchain.
		VkFormat SwapchainFormat;

		RenderStage Stage;

		inline void DrawStage(RenderStage* stage, VkCommandBuffer cmdBuffer, uint32_t iteration);

		RenderSystem3D* RSystem = nullptr;

	private:

		std::list<RenderStage*> SingleTimeStages = {};
		std::list<RenderStage*> Stages = {};

		void createSpriteVertexBuffer(Sprite* obj) const;

		void createSpriteIndexBuffer() const;

		bool checkValidationLayers();

		//Crea la instancia de Vulkan.
		void createInstance(const std::string& appName, const Version& gameVersion);

		//Establece la consola para las capas de validaci�n.
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

		OskResult createGraphicsPipeline2D();

		OskResult createGraphicsPipeline3D();

		void createCommandPool();

		void createDepthResources();

		void createFramebuffers();

		void createGlobalImageSampler();

		void createDefaultUniformBuffers();

		void createDescriptorPool();

		void createSyncObjects();

		void createCommandBuffers();

		void createRenderTarget();

		void closeSwapchain();

		void updateCommandBuffers();

		void updateSpriteVertexBuffer(Sprite* obj) const;

		VkCommandBuffer beginSingleTimeCommandBuffer() const;

		void endSingleTimeCommandBuffer(VkCommandBuffer cmdBuffer) const;

		//Obtiene el tipo de memoria indicado.
		uint32_t getMemoryType(const uint32_t& memoryTypeFilter, VkMemoryPropertyFlags flags) const;

		VULKAN::SwapchainSupportDetails getSwapchainSupportDetails(VkPhysicalDevice gpu) const;

		//Comprueba si una GPU es compatible.
		VULKAN::GPUinfo getGPUinfo(VkPhysicalDevice gpu) const;

		VULKAN::QueueFamilyIndices findQueueFamilies(VkPhysicalDevice gpu) const;

		VkFormat getSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

		//Obtiene el modo de presentaci�n:
		//C�mo se cambian las im�genes del swapchain.
		//
		//	VK_PRESENT_MODE_IMMEDIATE_KHR: se presentan directamente (tearing).
		//	VK_PRESENT_MODE_FIFO_KHR: VSync.
		//	VK_PRESENT_MODE_FIFO_RELAXED_KHR: VSync, tearing.
		//	VK_PRESENT_MODE_MAILBOX_KHR: "triple buffer".
		VkPresentModeKHR getPresentMode(const std::vector<VkPresentModeKHR>& modes) const;

		VkExtent2D getSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

		//VkDescriptorPool DescriptorPool;
		//VkDescriptorSetLayout DescriptorSetLayout;
		DescriptorLayout* DescLayout;

		std::vector<VulkanBuffer> UniformBuffers;

		//Debug messages.
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

		//Carga la funci�n necesaria para establecer la consola de capas de validaci�n.
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

		//Instancia de Vulkan.
		VkInstance Instance;

		//Swapchain.
		VkSwapchainKHR Swapchain;

		//Im�genes del swapchain.
		std::vector<VkImage> SwapchainImages;

		//ImageViews del swapchain (c�mo acceder a las im�genes).
		std::vector<VkImageView> SwapchainImageViews;

		//Tama�o del swapchain.
		VkExtent2D SwapchainExtent;

		//Modo de presentaci�n.
		PresentMode targetPresentMode = PresentMode::VSYNC;

		//GPU.
		VkPhysicalDevice GPU;

		//Superficie sobre la cual se renderiza las im�genes.
		VkSurfaceKHR Surface;

		//Logical device.
		VkDevice LogicalDevice;

		std::vector<VULKAN::Framebuffer*> Framebuffers;

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

		VkCommandPool CommandPool;

		VkSampler GlobalImageSampler;


		//Nos permite imprimir mensajes de las capas de validaci�n.
		VkDebugUtilsMessengerEXT debugConsole;

		struct {
			bool hasFramebufferBeenResized = false;
			uint32_t currentImage = 0;
		} renderVars;

		SpriteBatch currentSpriteBatch{};
		
		bool hasBeenInit = false;

		VkPhysicalDeviceMemoryProperties memProperties;

		DescriptorLayout* PhongDescriptorLayout = nullptr;
		DescriptorLayout* SkyboxDescriptorLayout = nullptr;

		/*NEW SYNC*/
		VkFence* fences = nullptr;

	};

}