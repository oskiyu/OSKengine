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
#include "DescriptorPool.h"
#include "LightsUBO.h"
#include "Skybox.h"

#include "ContentManager.h"
#include "RenderizableScene.h"

#include "VulkanImageGen.h"
#include "ProfilingUnit.h"
#include "Framebuffer.h"

#include "RenderTarget.h"
#include "RenderStage.h"
#include "PostProcessingSettings.h"
#include <functional>

#include "MaterialSystem.h"

#include "DynamicArray.hpp"

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
	
		~RenderAPI() {
			Close();
		}

		RenderTarget* RTarget = CreateNewRenderTarget();

		//Inicializa el renderizador.
		//	<appName>: nombre del juego.
		//	<gameVersion>: versión del juego.
		OskResult Init(const std::string& appName, const Version& gameVersion);

		//Renderiza el frame.
		void RenderFrame();

		//Establece el modo de presentación deseado.
		//Recrea el swapchain si es necesario.
		//	<mode>: modo de presentación objetivo.
		void SetPresentMode(PresentMode mode);

		//Obtiene el modo de presentación actual.
		PresentMode GetCurrentPresentMode() const;

		//Recarga los shaders.
		void ReloadShaders();


		//Cierra el renderizador.		
		void Close();


		//Establece el spriteBatch que se va a renderizar al llamar a RenderFrame().
		//	<spriteBatch>: spriteBatch que se renderizará.
		void AddSpriteBatch(SpriteBatch* spriteBatch);

		void RemoveSpriteBatch(SpriteBatch* spriteBatch);

		//Recrea el swapchain.
		void RecreateSwapchain();


		//Crea un spriteBatch.
		SpriteBatch CreateSpriteBatch();

		Material* CreateNewMaterial() {
			Material* mat = new Material();
			mat->SetRenderer(this);

			return mat;
		}

		//Crea un nuevo graphics pipeline vacío.
		//	<vertexPath>: ruta del shader de vértices.
		//	<fragmentPath>: ruta del shader de fragmento.
		GraphicsPipeline* CreateNewGraphicsPipeline(const std::string& vertexPath, const std::string& fragmentPath) const;

		//Crea un nuevo descriptor layout vacío.
		DescriptorPool* CreateNewDescriptorPool() const;

		//Crea un nuevo descriptor layout vacío.
		DescriptorLayout* CreateNewDescriptorLayout() const;

		//Crea un nuevo descriptor set vacío.
		DescriptorSet* CreateNewDescriptorSet() const;
				
		//Crea un buffer que almacenará información en la GPU.
		//	<usage>: el uso que se le dará al buffer.
		//	<prop>: propiedades de memoria que necesitará el buffer.
		//
		VulkanBuffer CreateBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags prop) const;

		//Crea un buffer que almacenará información en la GPU.
		//	<buffer>: buffer que se va a crear.
		//	<size>: tamaño del buffer.
		//	<usage>: el uso que se le dará al buffer.
		//	<prop>: propiedades de memoria que necesitará el buffer.
		//
		//Para destruir un buffer, llamar a VulkanRenderer::DestroyBuffer.
		void CreateDynamicUBO(VulkanBuffer& buffer, VkDeviceSize sizeOfStruct, uint32_t numberOfInstances) const;

		//Copia el contenido de un buffer a otro buffer.
		//	<source>: buffer fuente.
		//	<destination>: buffer al que se copiará la información.
		//	<size>: tamaño de la información que se va a copiar (puede no ser el mismo tamaño que el de los buffers).
		//	<sourceOffset = 0>: offset sobre la informacíon del buffer fuente.
		//	<destinationOffset = 0>: offset sobre el buffer destino.
		void CopyBuffer(VulkanBuffer& source, VulkanBuffer& destination, VkDeviceSize size, VkDeviceSize sourceOffset = 0, VkDeviceSize destinationOffset = 0) const;

		VULKAN::Renderpass* CreateNewRenderpass();

		RenderTarget* CreateNewRenderTarget();
		void InitRenderTarget(RenderTarget* rtarget, ContentManager* content);

		VULKAN::Framebuffer* CreateNewFramebuffer();

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
		std::vector<VulkanBuffer> UniformBuffers{};

		//Camaras.
		Camera2D DefaultCamera2D{};
		Camera2D RenderTargetCamera2D{};
		Camera3D DefaultCamera3D{0, 0, 0};

		//Ventana asociada.
		WindowAPI* Window = nullptr;

		//Límite de FPS.
		float FPSlimit = 9999.0f;

		VULKAN::Renderpass* renderpass;

		ContentManager* Content = new ContentManager(this);

		RenderizableScene* Scene;

		ProfilingUnit renderP_Unit{ "Render frame" };
		ProfilingUnit updateCmdP_Unit{ "Update command buffers" };

		Sprite OSKengineIconSprite;
		Sprite OSK_IconSprite;

		unsigned int RenderTargetSizeX = 1024;
		unsigned int RenderTargetSizeY = 720;
		float RenderResolutionMultiplier = 1.0f; //Record = 17.0f { 32640 x 18360 }

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

		PostProcessingSettings_t PostProcessingSettings;

		void SetViewport(VkCommandBuffer& cmdBuffer, int32_t x = 0, int32_t y = 0, uint32_t sizeX = 0, uint32_t sizeY = 0) const;

		const std::string DefaultMaterial2D_Name = "DefaultMaterial2D";
		const std::string DefaultMaterial3D_Name = "DefaultMaterial3D";
		const std::string DefaultSkyboxMaterial_Name = "DefaultSkyboxMaterial";
		const std::string DefaultShadowsMaterial_Name = "DefaultShadowsMaterial";

		MaterialSystem* GetMaterialSystem() {
			return MSystem;
		}

	private:

		MaterialSystem* MSystem = nullptr;

		std::list<RenderStage*> SingleTimeStages = {};
		std::list<RenderStage*> Stages = {};

		void createSpriteVertexBuffer(Sprite* obj) const;

		void createSpriteIndexBuffer() const;

		bool checkValidationLayers();

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
		void updateSpriteVertexBuffer(SpriteContainer& obj) const;

		VkCommandBuffer beginSingleTimeCommandBuffer() const;

		void endSingleTimeCommandBuffer(VkCommandBuffer cmdBuffer) const;

		//Obtiene el tipo de memoria indicado.
		uint32_t getMemoryType(const uint32_t& memoryTypeFilter, VkMemoryPropertyFlags flags) const;

		VULKAN::SwapchainSupportDetails getSwapchainSupportDetails(VkPhysicalDevice gpu) const;

		//Comprueba si una GPU es compatible.
		VULKAN::GPUinfo getGPUinfo(VkPhysicalDevice gpu) const;

		VULKAN::QueueFamilyIndices findQueueFamilies(VkPhysicalDevice gpu) const;

		VkFormat getSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

		//Obtiene el modo de presentación:
		//Cómo se cambian las imágenes del swapchain.
		//
		//	VK_PRESENT_MODE_IMMEDIATE_KHR: se presentan directamente (tearing).
		//	VK_PRESENT_MODE_FIFO_KHR: VSync.
		//	VK_PRESENT_MODE_FIFO_RELAXED_KHR: VSync, tearing.
		//	VK_PRESENT_MODE_MAILBOX_KHR: "triple buffer".
		VkPresentModeKHR getPresentMode(const std::vector<VkPresentModeKHR>& modes) const;

		VkExtent2D getSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

		//Debug messages.
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

		//Carga la función necesaria para establecer la consola de capas de validación.
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

		//Instancia de Vulkan.
		VkInstance Instance;

		//Swapchain.
		VkSwapchainKHR Swapchain;

		//Imágenes del swapchain.
		std::vector<VkImage> SwapchainImages;

		//ImageViews del swapchain (cómo acceder a las imágenes).
		std::vector<VkImageView> SwapchainImageViews;

		//Tamaño del swapchain.
		VkExtent2D SwapchainExtent;

		//Modo de presentación.
		PresentMode targetPresentMode = PresentMode::VSYNC;

		//GPU.
		VkPhysicalDevice GPU;
		VULKAN::GPUinfo GPU_Info{};
		VkPhysicalDeviceMemoryProperties MemoryProperties;

		//Superficie sobre la cual se renderiza las imágenes.
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


		//Nos permite imprimir mensajes de las capas de validación.
		VkDebugUtilsMessengerEXT debugConsole;

		struct {
			bool hasFramebufferBeenResized = false;
			uint32_t currentImage = 0;
		} renderVars;

		SpriteBatch currentSpriteBatch{};
		
		bool hasBeenInit = false;

		VkPhysicalDeviceMemoryProperties memProperties;

		//POST-PROCESSING
		GraphicsPipeline* ScreenGraphicsPipeline = nullptr;
		DescriptorPool* ScreenDescriptorPool = nullptr;
		DescriptorLayout* ScreenDescriptorLayout = nullptr;
		DescriptorSet* ScreenDescriptorSet = nullptr;
		VULKAN::Renderpass* ScreenRenderpass = nullptr;

		RenderTarget* FinalRenderTarget = nullptr;

		void InitPostProcessing();
		void RecreatePostProcessing();
		void ClosePostProcessing();

		//Animation
		struct {
			void AddAnimatedModel(AnimatedModel* model) {
				if (!FreeSpaces.IsEmpty()) {
					uint32_t index = FreeSpaces.Pop();
					Models.Insert(model);
					model->AnimationBufferOffset = index;

					return;
				}
				model->AnimationBufferOffset = AnimationCount;
				AnimationCount++;
				Models.Insert(model);
			}

			void RemoveAnimatedModel(AnimatedModel* model) {
				if (!Models.HasElement(model))
					return;

				uint32_t index = Models.GetPosition(model);
				FreeSpaces.Push(index);

				Models.Remove(index);
			}

			DynamicArray<AnimatedModel*> Models{};
			uint32_t AnimationCount = 1;
			std::vector<AnimUBO> BoneUBOs{};
			Stack<uint32_t> FreeSpaces;
			
		} AnimationSystem;

		/*NEW SYNC*/
		VkFence* fences = nullptr;

	};

}