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
#include "Camera3D.h"
#include <unordered_map>
#include "Font.h"
#include "AnimatedModel.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Renderpass.h"
#include "Model.h"
#include "GraphicsPipeline.h"
#include "DescriptorLayout.h"
#include "LightsUBO.h"
#include "Skybox.h"

namespace OSK {

	class OSKAPI_CALL VulkanRenderer {

	public:
	

		//Inicializa el renderizador.
		//	<mode>: modo de renderizado (2D / 2D + 3D).
		//	<appName>: nombre del juego.
		//	<gameVersion>: versión del juego.
		OskResult Init(const RenderMode& mode, const std::string& appName, const Version& gameVersion);

		//Renderiza el frame.
		void RenderFrame();

		//Renderiza la interfaz de usuario de la ventana asociada.
		void DrawUserInterface(SpriteBatch& spriteBatch) const;


		//Establece el modo de presentación deseado.
		//Recrea el swapchain si es necesario.
		//	<mode>: modo de presentación objetivo.
		void SetPresentMode(const PresentMode& mode);

		//Obtiene el modo de presentación actual.
		PresentMode GetCurrentPresentMode() const;

		//Recarga los shaders.
		void ReloadShaders();


		//Cierra el renderizador.		
		void Close();


		//Carga una textura 2D (para un sprite).
		//	<path>: ruta de la textura (incluyendo la extensión de la imagen).
		Texture* LoadTexture(const std::string& path);


		//Carga una textura 2D (para un modelo 3D).
		//	<path>: ruta de la textura (incluyendo la extensión de la imagen).
		ModelTexture* LoadModelTexture(const std::string& path);


		//Carga los vértices y los índices de un modelo 3D.
		//	<path>: ruta del modelo (incluyendo la extensión del modelo).
		//	<scala>: escala del modelo 3D.
		TempModelData GetModelTempData(const std::string& path, const float_t& scale = 1.0f) const;


		//Carga un modelo 3D con sus buffers creados.
		//Solamente existe una instancia de ModelData* por cada <path>.
		//	<path>: ruta del modelo (incluyendo la extensión del modelo).
		//	<scala>: escala del modelo 3D.
		ModelData* LoadModelData(const std::string& path, const float_t& scale = 1.0f);


		//Carga un modelo 3D.
		//Las texturas del modelo deben estar en la misma carpeta que el modelo.
		//	<model>: modelo a cargar.
		//	<path>: ruta del modelo (incluyendo la extensión del modelo).
		void LoadModel(Model& model, const std::string& path);

		void LoadSkybox(Skybox& skybox, const std::string& path);


		//Crea un modelo 3D a partir de sus vértices y sus índices.
		//	<vértices>: vértices.
		//	<índices>: índices (vertexIndex_t).
		ModelData* CreateModel(const std::vector<Vertex>& vertices, const std::vector<vertexIndex_t>& indices);


		//Carga un modelo 3D animado.
		//	<model>: modelo animado a cargar.
		//	<path>: ruta del modelo (incluyendo la extensión del modelo).
		void LoadAnimatedModel(AnimatedModel& model, const std::string& path);


		//Carga un sprite.
		//	<texture>: sprite a cargar.
		//	<path>: ruta de la textura (incluyendo la extensión).
		void LoadSprite(Sprite* texture, const std::string& path);

		//Establece el spriteBatch que se va a renderizar al llamar a RenderFrame().
		//	<spriteBatch>: spriteBatch que se renderizará.
		void SubmitSpriteBatch(const SpriteBatch& spriteBatch);

		//Recrea el swapchain.
		void RecreateSwapchain();

		//Carga una fuente.
		//	<font>: fuente a cargar.
		//	<source>: ruta del archivo (incluyendo la extensión).
		//	<size>: tamaño al que se va a cargar la fuente, en píxeles.
		void LoadFont(Font& fuente, const std::string& source, uint32_t size);

		//Crea un spriteBatch.
		SpriteBatch CreateSpriteBatch();


		//Crea un nuevo graphics pipeline vacío.
		//	<vertexPath>: ruta del shader de vértices.
		//	<fragmentPath>: ruta del shader de fragmento.
		GraphicsPipeline* CreateNewGraphicsPipeline(const std::string& vertexPath, const std::string& fragmentPath) const;

		//Crea un nuevo descriptor layout vacío.
		DescriptorLayout* CreateNewDescriptorLayout() const;

		//Crea un nuevo descriptor set vacío.
		DescriptorSet* CreateNewDescriptorSet() const;
		
		//Phong lighting.
		
		//Crea un nuevo graphics pipeline configurado para el motor de iluminación PHONG.
		//	<vertexPath>: ruta del shader PHONG de vértices.
		//	<fragmentPath>: ruta del shader PHONG de fragmento.
		GraphicsPipeline* CreateNewPhongPipeline(const std::string& vertexPath, const std::string& fragmentPath) const;

		//Crea un nuevo descriptor layout configurado para usarse con el motor de iluminación PHONG.
		DescriptorLayout* CreateNewPhongDescriptorLayout() const;

		//Crea el descriptor set de PHONG para la textura de un modelo 3D.
		//	<texture>: textura a la que se le configurará su descriptor set de PHONG.
		void CreateNewPhongDescriptorSet(ModelTexture* texture) const;

		//Skybox.
		GraphicsPipeline* CreateNewSkyboxPipeline(const std::string& vertexPath, const std::string& fragmentPath) const;
		DescriptorLayout* CreateNewSkyboxDescriptorLayout() const;
		void CreateNewSkyboxDescriptorSet(SkyboxTexture* texture) const;
		
		//Crea un buffer que almacenará información en la GPU.
		//	<buffer>: buffer que se va a crear.
		//	<size>: tamaño del buffer.
		//	<usage>: el uso que se le dará al buffer.
		//	<prop>: propiedades de memoria que necesitará el buffer.
		//
		//Para destruir un buffer, llamar a VulkanRenderer::DestroyBuffer.
		void CreateBuffer(VulkanBuffer& buffer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags prop) const;

		//Destruye un buffer, liberando la memoria que tenía asignada en la GPU.
		//	<buffer>: buffer a destruir.
		void DestroyBuffer(VulkanBuffer& buffer) const;

		//Copia el contenido de un buffer a otro buffer.
		//	<source>: buffer fuente.
		//	<destination>: buffer al que se copiará la información.
		//	<size>: tamaño de la información que se va a copiar (puede no ser el mismo tamaño que el de los buffers).
		//	<sourceOffset = 0>: offset sobre la informacíon del buffer fuente.
		//	<destinationOffset = 0>: offset sobre el buffer destino.
		void CopyBuffer(VulkanBuffer& source, VulkanBuffer& destination, VkDeviceSize size, VkDeviceSize sourceOffset = 0, VkDeviceSize destinationOffset = 0) const;

		struct {
			std::string VertexShaderPath2D = "Shaders/2D/vert.spv";
			std::string FragmentShaderPath2D = "Shaders/2D/frag.spv";

			std::string VertexShaderPath3D = "Shaders/Vk/vert.spv";
			std::string FragmentShaderPath3D = "Shaders/Vk/frag.spv";

			uint32_t MaxTextures = 32;

			bool AutoUpdateCommandBuffers = true;
		} Settings;


		std::vector<VkCommandBuffer> CommandBuffers;

		//Camera bidimensional.
		Camera2D DefaultCamera2D{};
		Camera3D DefaultCamera3D{0, 0, 0};

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

		void createImage(VULKAN::VulkanImage* image, const uint32_t& width, const uint32_t& height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

		void createImageView(VULKAN::VulkanImage* img, VkFormat format, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT, VkImageViewType type = VK_IMAGE_VIEW_TYPE_2D) const;

		void destroyImage(VULKAN::VulkanImage* img) const;

		void closeSwapchain();

		void copyBufferToImage(VulkanBuffer* buffer, VULKAN::VulkanImage* img, const uint32_t& width, const uint32_t& height) const;

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

		VkCommandBuffer beginSingleTimeCommandBuffer() const;

		void endSingleTimeCommandBuffer(VkCommandBuffer cmdBuffer) const;

		//Obtiene el tipo de memoria indicado.
		uint32_t getMemoryType(const uint32_t& memoryTypeFilter, VkMemoryPropertyFlags flags) const;

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

		//VkDescriptorPool DescriptorPool;
		//VkDescriptorSetLayout DescriptorSetLayout;
		DescriptorLayout* DescLayout;

		std::vector<VulkanBuffer> UniformBuffers;

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
		VULKAN::Renderpass* renderpass;

		GraphicsPipeline* GraphicsPipeline2D;
		GraphicsPipeline* GraphicsPipeline3D;
		GraphicsPipeline* SkyboxGraphicsPipeline;

		VkCommandPool CommandPool;

		VkSampler GlobalImageSampler;


		//Nos permite imprimir mensajes de las capas de validación.
		VkDebugUtilsMessengerEXT debugConsole;

		struct {
			bool hasFramebufferBeenResized = false;
			uint32_t currentImage = 0;
		} renderVars;

		SpriteBatch currentSpriteBatch{};
		
		std::unordered_map<std::string, Texture*> textureFromString = {};
		std::vector<Texture> textures = {};
		std::vector<ModelData*> modelDatas = {};
		std::unordered_map<std::string, ModelData*> modelDataFromPath = {};

		std::vector<Sprite*> sprites = {};

		bool hasBeenInit = false;

		static Assimp::Importer GlobalImporter;
		const static int AssimpFlags = aiProcess_Triangulate | aiProcess_GenNormals;

		VkPhysicalDeviceMemoryProperties memProperties;

		DescriptorLayout* PhongDescriptorLayout = nullptr;
		DescriptorLayout* SkyboxDescriptorLayout = nullptr;

		//game
		LightUBO lights{};
		std::vector<VulkanBuffer> LightsUniformBuffers;

		/*NEW SYNC*/
		VkFence* fences = nullptr;
	};

}