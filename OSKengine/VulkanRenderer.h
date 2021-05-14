#pragma once

#include <vulkan/vulkan.h>

#include "PresentMode.h"
#include <string>
#include "WindowAPI.h"
#include "Version.h"
#include "GPUinfo.h"
#include "GPUImage.h"
#include "GPUDataBuffer.h"
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

	/// <summary>
	/// Renderizador de OSKengine.
	/// </summary>
	class RenderSystem3D;

	class OSKAPI_CALL RenderAPI {

		friend class ContentManager;
		friend class RenderizableScene;
		friend class ShadowMap;
		friend class CubeShadowMap;
		friend class VULKAN::VulkanImageGen;
		friend class VULKAN::Framebuffer;

	public:

		/// <summary>
		/// Cierra el renderizador.
		/// </summary>
		~RenderAPI() {
			Close();
		}

#pragma region PublicAPI

		/// <summary>
		/// Inicializa el renderizador.
		/// </summary>
		/// <param name="appName">Nombre del juego.</param>
		/// <param name="gameVersion">Versi�n del juego.</param>
		void Init(const std::string& appName, const Version& gameVersion);

		/// <summary>
		/// Renderiza el frame.
		/// </summary>
		void RenderFrame();

		/// <summary>
		/// Establece el modo de presentaci�n deseado.
		/// Recrea el swapchain si es necesario.
		/// </summary>
		/// <param name="mode">Modo de presentaci�n objetivo.</param>
		void SetPresentMode(PresentMode mode);

		/// <summary>
		/// Obtiene el modo de presentaci�n actual.
		/// </summary>
		/// <returns>Modo de presentaci�n.</returns>
		PresentMode GetCurrentPresentMode() const;

		/// <summary>
		/// Recarga los shaders.
		/// </summary>
		void ReloadShaders();

		/// <summary>
		/// Crea un spriteBatch.
		/// </summary>
		/// <returns>Spritebatch usable.</returns>
		SpriteBatch CreateSpriteBatch();

		/// <summary>
		/// Crea un buffer que almacenar� informaci�n en la GPU.
		/// </summary>
		/// <param name="usage">El uso que se le dar� al buffer.</param>
		/// <param name="prop">Propiedades de memoria que necesitar� el buffer.</param>
		/// <returns>Buffer usable.</returns>
		GPUDataBuffer CreateBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags prop) const;

		/// <summary>
		/// Crea un buffer que almacenar� un dynamic ubo.
		/// </summary>
		/// <param name="buffer">Buffer que se va a crear.</param>
		/// <param name="sizeOfStruct">Tama�o de cada estructura individual.</param>
		/// <param name="numberOfInstances">N�mero de estructuras a almacenar.</param>
		void CreateDynamicUBO(GPUDataBuffer& buffer, VkDeviceSize sizeOfStruct, uint32_t numberOfInstances) const;

		/// <summary>
		/// Copia el contenido de un buffer a otro buffer.
		/// </summary>
		/// <param name="source">Buffer fuente.</param>
		/// <param name="destination">Buffer al que se copiar� la informaci�n.</param>
		/// <param name="size">Tama�o de la informaci�n que se va a copiar (puede no ser el mismo tama�o que el de los buffers).</param>
		/// <param name="sourceOffset">Offset sobre la informac�on del buffer fuente.</param>
		/// <param name="destinationOffset">Offset sobre el buffer destino.</param>
		void CopyBuffer(GPUDataBuffer& source, GPUDataBuffer& destination, VkDeviceSize size, VkDeviceSize sourceOffset = 0, VkDeviceSize destinationOffset = 0) const;

		/// <summary>
		/// Obtiene el render target andes de postprocesamiento.
		/// </summary>
		/// <returns>Render target.</returns>
		inline RenderTarget* GetMainRenderTarget() const {
			return renderTargetBeforePostProcessing;
		}

		/// <summary>
		/// Crea un nuevo render target vac�o.
		/// </summary>
		/// <returns>Render target.</returns>
		RenderTarget* CreateNewRenderTarget();

		/// <summary>
		/// Inicializa un render target para su uso.
		/// Renderiza una imagen a color (sin depth).
		/// Tama�o = resoluci�n de la ventana.
		/// </summary>
		/// <param name="rtarget">Render target a inicializar..</param>
		/// <param name="content">Content manager para crear el sprite.</param>
		void InitRenderTarget(RenderTarget* rtarget, ContentManager* content);

		struct {
			std::string vertexShaderPath2D = "Shaders/2D/vert.spv";
			std::string fragmentShaderPath2D = "Shaders/2D/frag.spv";

			std::string vertexShaderPath3D = "Shaders/Vk/vert.spv";
			std::string fragmentShaderPath3D = "Shaders/Vk/frag.spv";

			std::string skyboxVertexPath = "shaders/VK_Skybox/vert.spv";
			std::string skyboxFragmentPath = "shaders/VK_Skybox/frag.spv";
		} Settings;

		/// <summary>
		/// C�mara 2D por defecto.
		/// </summary>
		Camera2D defaultCamera2D{};

		/// <summary>
		/// C�ramara 2D para el render target.
		/// </summary>
		Camera2D renderTargetCamera2D{};

		/// <summary>
		/// C�mara 3D por defecto.
		/// </summary>
		Camera3D defaultCamera3D{ 0, 0, 0 };

		/// <summary>
		/// Imagen de OSKengine.
		/// </summary>
		Sprite OSKengineIconSprite;

		/// <summary>
		/// Imagen de OSK.
		/// </summary>
		Sprite OSK_IconSprite;

		/// <summary>
		/// L�mite de FPS (no funciona).
		/// </summary>
		float fpsLimit = 9999.0f;

		/// <summary>
		/// Resoluci�n objetivo en X.
		/// Deve coincidir con la resoluci�n nativa de la pantalla
		/// </summary>
		unsigned int renderTargetSizeX = 1024;

		/// <summary>
		/// Resoluci�n objetivo en Y.
		/// Deve coincidir con la resoluci�n nativa de la pantalla
		/// </summary>
		unsigned int renderTargetSizeY = 720;

		/// <summary>
		/// Escala de renderizado.
		/// Debe ser al menos del 100% (1.0f).
		/// Si es mayor, renderiza a una resoluci�n superior al output, lo cual puede proveeer efectos de antailasing.
		/// </summary>
		float renderResolutionMultiplier = 1.0f; //Record = 17.0f { 32640 x 18360 }

		/// <summary>
		/// A�ade un render stage que se renderizar� todos los frames.
		/// Los render stage se renderizan en el orden que se han introducido.
		/// </summary>
		/// <param name="stage">Render stage.</param>
		void AddStage(RenderStage* stage);

		/// <summary>
		/// Quita un render stage para no renedrizarse m�s.
		/// </summary>
		/// <param name="stage">Render stage.</param>
		void RemoveStage(RenderStage* stage);

		/// <summary>
		/// A�ade un render stage que se renderizar� en el pr�ximo frame.
		/// </summary>
		/// <param name="stage">Render stage.</param>
		void AddSingleTimeStage(RenderStage* stage);

		/// <summary>
		/// Ajustes de postprocesamiento.
		/// </summary>
		PostProcessingSettings_t postProcessingSettings;

		/// <summary>
		/// Nombre del material 2D.
		/// </summary>
		const std::string defaultMaterial2D_Name = "DefaultMaterial2D";

		/// <summary>
		/// Nombre del material 3D.
		/// </summary>
		const std::string defaultMaterial3D_Name = "DefaultMaterial3D";

		/// <summary>
		/// Nombre del material de skybox.
		/// </summary>
		const std::string defaultSkyboxMaterial_Name = "DefaultSkyboxMaterial";

		/// <summary>
		/// Nombre del material de sombreado.
		/// </summary>
		const std::string defaultShadowsMaterial_Name = "DefaultShadowsMaterial";

		/// <summary>
		/// Devuelve el sistema de materiales.
		/// </summary>
		/// <returns></returns>
		MaterialSystem* GetMaterialSystem() const {
			return materialSystem;
		}

		/// <summary>
		/// Obtiene los uniform buffers 3D.
		/// </summary>
		/// <returns>UBOs 3D.</returns>
		std::vector<GPUDataBuffer>& GetUniformBuffers() {
			return uniformBuffers;
		}

#pragma endregion

		/// <summary>
		/// Render target antes de post procesamiento.
		/// </summary>
		RenderTarget* renderTargetBeforePostProcessing = CreateNewRenderTarget();

		/// <summary>
		/// Cierra el renderizador.
		/// </summary>
		void Close();

		/// <summary>
		/// Recrea el swapchain.
		/// </summary>
		void RecreateSwapchain();
		
#pragma region Builders

		/// <summary>
		/// Crea un nuevo graphics pipeline vac�o.
		/// </summary>
		/// <param name="vertexPath">Ruta del shader de v�rtices.</param>
		/// <param name="fragmentPath">Ruta del shader de fragmento.</param>
		/// <returns>Graphics pipeline 'vac�o'.</returns>
		GraphicsPipeline* CreateNewGraphicsPipeline(const std::string& vertexPath, const std::string& fragmentPath) const;

		/// <summary>
		/// Crea un nuevo descriptor layout vac�o.
		/// </summary>
		/// <returns>Descriptor pool vac�o.</returns>
		DescriptorPool* CreateNewDescriptorPool() const;

		//Crea un nuevo descriptor layout vac�o.

		/// <summary>
		/// Crea un nuevo descriptor layout vac�o.
		/// </summary>
		/// <returns>Descriptor layout vac�o.</returns>
		DescriptorLayout* CreateNewDescriptorLayout() const;

		/// <summary>
		/// Crea un nuevo descriptor set vac�o.
		/// </summary>
		/// <returns>Descriptor set vac�o.</returns>
		DescriptorSet* CreateNewDescriptorSet() const;
				
		/// <summary>
		/// Crea un renderpass vac�o.
		/// </summary>
		/// <returns>Renderpass.</returns>
		VULKAN::Renderpass* CreateNewRenderpass();

		/// <summary>
		/// Crea un framebuffer vac�o.
		/// </summary>
		/// <returns>Framebuffer.</returns>
		VULKAN::Framebuffer* CreateNewFramebuffer();

#pragma endregion

		/// <summary>
		/// Ventana asociada.
		/// </summary>
		WindowAPI* window = nullptr;

		/// <summary>
		/// Content manager global.
		/// </summary>
		ContentManager* content = new ContentManager(this);

		/// <summary>
		/// Profiling para el renderizado del frame (GPU).
		/// </summary>
		ProfilingUnit renderProfilingUnit{ "Render frame" };

		/// <summary>
		/// Profiling para el renderizado (CPU).
		/// </summary>
		ProfilingUnit updateCmdProfilingUnit{ "Update command buffers" };

		/// <summary>
		/// Imagen de profundidad.
		/// </summary>
		VULKAN::GPUImage depthImage;

		/// <summary>
		/// Devuelve el formato para la imagen de profundidad.
		/// </summary>
		/// <returns>Formato.</returns>
		VkFormat getDepthFormat() const;

		/// <summary>
		/// Formato del swapchain.
		/// </summary>
		VkFormat swapchainFormat;

		/// <summary>
		/// Renderiza un render stage.
		/// </summary>
		/// <param name="stage">Render stage.</param>
		/// <param name="cmdBuffer">Command buffer.</param>
		/// <param name="iteration">Iteraci�n.</param>
		void DrawStage(RenderStage* stage, VkCommandBuffer cmdBuffer, uint32_t iteration);

		/// <summary>
		/// Sistema ECS de renderizado.
		/// </summary>
		RenderSystem3D* renderSystem = nullptr;

		/// <summary>
		/// Establece el viewport para un command buffer.
		/// Por defecto, el tama�o de la ventana.
		/// </summary>
		/// <param name="cmdBuffer">Command buffer.</param>
		/// <param name="x">Posici�n X.</param>
		/// <param name="y">Posici�n Y.</param>
		/// <param name="sizeX">Tama�o X.</param>
		/// <param name="sizeY">Tama�o Y.</param>
		void SetViewport(VkCommandBuffer& cmdBuffer, int32_t x = 0, int32_t y = 0, uint32_t sizeX = 0, uint32_t sizeY = 0) const;

	private:

		/// <summary>
		/// Command buffers.
		/// </summary>
		std::vector<VkCommandBuffer> commandBuffers;

		/// <summary>
		/// UBOs 3D.
		/// </summary>
		std::vector<GPUDataBuffer> uniformBuffers{};

		/// <summary>
		/// Sistema de materiales.
		/// </summary>
		MaterialSystem* materialSystem = nullptr;

		/// <summary>
		/// Stages que se renderizan una vez por frame.
		/// Despues la lista se vac�a.
		/// </summary>
		std::list<RenderStage*> singleTimeStages = {};

		/// <summary>
		/// Stages a renderizar cada frame.
		/// </summary>
		std::list<RenderStage*> stages = {};

		/// <summary>
		/// Crea el vertex buffer de un sprite.
		/// </summary>
		/// <param name="obj">Sprite.</param>
		void createSpriteVertexBuffer(Sprite* obj) const;

		/// <summary>
		/// Crea el index buffer de los sprites.
		/// </summary>
		void createSpriteIndexBuffer() const;

		/// <summary>
		/// Comprueba el soporte de las capas de validaci�n.
		/// </summary>
		/// <returns>True si se soportan.</returns>
		bool checkValidationLayers();

		/// <summary>
		/// Crea la instancia de Vulkan.
		/// </summary>
		/// <param name="appName">Nombre del juego.</param>
		/// <param name="gameVersion">Versi�n del juego.</param>
		void createInstance(const std::string& appName, const Version& gameVersion);

		/// <summary>
		/// Establece la consola para las capas de validaci�n.
		/// </summary>
		void setupDebugConsole();

		/// <summary>
		/// Crea la suprficie de renerizado ('pantalla').
		/// </summary>
		void createSurface();

		/// <summary>
		/// Selecciona la GPU que se va a usar.
		/// </summary>
		void getGPU();

		/// <summary>
		/// Crea el logical device.
		/// </summary>
		void createLogicalDevice();

		/// <summary>
		/// Crea el swapchain.
		/// </summary>
		void createSwapchain();

		/// <summary>
		/// Crea las im�genes del swapchain.
		/// </summary>
		void createSwapchainImageViews();

		/// <summary>
		/// Crea el renderpass principal.
		/// </summary>
		void createRenderpass();

		/// <summary>
		/// Crea el command pool.
		/// </summary>
		void createCommandPool();

		/// <summary>
		/// Crea la imagen de profundidad.
		/// </summary>
		void createDepthResources();

		/// <summary>
		/// Crea los framebuffers.
		/// </summary>
		void createFramebuffers();

		/// <summary>
		/// Crea un image sampler global.
		/// </summary>
		void createGlobalImageSampler();

		/// <summary>
		/// Crea los UBOs 3D.
		/// </summary>
		void createDefaultUniformBuffers();

		/// <summary>
		/// Crea las barreras, sem�foros, etc.
		/// </summary>
		void createSyncObjects();

		/// <summary>
		/// Crea los command buffers.
		/// </summary>
		void createCommandBuffers();

		/// <summary>
		/// Crea el render target antes de postprocesamiento.
		/// </summary>
		void createRenderTarget();

		/// <summary>
		/// Cierra el swapchain.
		/// </summary>
		void closeSwapchain();

		/// <summary>
		/// Renderizado.
		/// </summary>
		void updateCommandBuffers();

		/// <summary>
		/// Actualiza el vertex buffer de un sprite.
		/// </summary>
		/// <param name="obj">Sprite.</param>
		void updateSpriteVertexBuffer(Sprite* obj) const;

		/// <summary>
		/// Actualiza el vertex buffer de un sprite.
		/// </summary>
		/// <param name="obj">Sprite.</param>
		void updateSpriteVertexBuffer(SpriteContainer& obj) const;

		/// <summary>
		/// Crea y devuelve un command buffer para ser usado una sola vez.
		/// </summary>
		/// <returns></returns>
		VkCommandBuffer beginSingleTimeCommandBuffer() const;

		/// <summary>
		/// Env�a a la GPU un buffer de comandos de un solo uso.
		/// </summary>
		/// <param name="cmdBuffer">Command buffer.</param>
		void endSingleTimeCommandBuffer(VkCommandBuffer cmdBuffer) const;

		/// <summary>
		/// Obtiene el tipo de memoria indicado.
		/// </summary>
		/// <param name="memoryTypeFilter">Tipo.</param>
		/// <param name="flags">Usos.</param>
		/// <returns>Tipo de memoria de la GPU.</returns>
		uint32_t getMemoryType(const uint32_t& memoryTypeFilter, VkMemoryPropertyFlags flags) const;

		/// <summary>
		/// Obtiene los detalles de soporte de swapchain para una GPU en concreto.
		/// </summary>
		/// <param name="gpu">GPU.</param>
		/// <returns>Detalles de soporte.</returns>
		VULKAN::SwapchainSupportDetails getSwapchainSupportDetails(VkPhysicalDevice gpu) const;

		/// <summary>
		/// Obtiene informaci�n importante de una GPU.
		/// </summary>
		/// <param name="gpu">GPU.</param>
		/// <returns>Informaci�n.</returns>
		VULKAN::GPUinfo getGPUinfo(VkPhysicalDevice gpu) const;

		/// <summary>
		/// Obtiene las familias de queues soportadas por una GPU.
		/// </summary>
		/// <param name="gpu">GPU.</param>
		/// <returns>Detalles.</returns>
		VULKAN::QueueFamilyIndices findQueueFamilies(VkPhysicalDevice gpu) const;

		/// <summary>
		/// Obtiene el mejor formato de color de entre los disponibles.
		/// </summary>
		/// <param name="candidates">Disponibles.</param>
		/// <param name="tiling">Tiling de la imagen.</param>
		/// <param name="features">Caracter�sticas requeridas.</param>
		/// <returns>Mejor formato.</returns>
		VkFormat getSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

		/// <summary>
		/// Obtiene el modo de presentaci�n:
		/// c�mo se cambian las im�genes del swapchain.
		/// <para/>
		/// <para/>
		/// VK_PRESENT_MODE_IMMEDIATE_KHR: se presentan directamente (tearing).<para/>
		/// VK_PRESENT_MODE_FIFO_KHR: VSync.<para/>
		/// VK_PRESENT_MODE_FIFO_RELAXED_KHR: VSync, tearing.<para/>
		/// VK_PRESENT_MODE_MAILBOX_KHR: "triple buffer".<para/>
		/// </summary>
		/// <param name="modes">Modos disponibles.</param>
		/// <returns>Modo de presentaci�n.</returns>
		VkPresentModeKHR getPresentMode(const std::vector<VkPresentModeKHR>& modes) const;

		/// <summary>
		/// Obtiene el tama�o del swapchain.
		/// </summary>
		/// <param name="capabilities">Superficie.</param>
		/// <returns>Tama�o.</returns>
		VkExtent2D getSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

		/// <summary>
		/// Callbag para logear los mensajes de Vulkan.
		/// </summary>
		/// <param name="messageSeverity">Importancia del mensaje.</param>
		/// <param name="messageType">Tipo de mensaje</param>
		/// <param name="pCallbackData"></param>
		/// <param name="pUserData"></param>
		/// <returns></returns>
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

		//Carga la funci�n necesaria para establecer la consola de capas de validaci�n.
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

		/// <summary>
		/// Instancia de Vulkan.
		/// </summary>
		VkInstance instance;

		/// <summary>
		/// Swapchain.
		/// </summary>
		VkSwapchainKHR swapchain;

		/// <summary>
		/// Im�genes del swapchain.
		/// </summary>
		std::vector<VkImage> swapchainImages;

		/// <summary>
		/// ImageViews del swapchain (c�mo acceder a las im�genes).
		/// </summary>
		std::vector<VkImageView> swapchainImageViews;

		/// <summary>
		/// Tama�o del swapchain.
		/// </summary>
		VkExtent2D swapchainExtent;

		/// <summary>
		/// Modo de presentaci�n.
		/// </summary>
		PresentMode targetPresentMode = PresentMode::VSYNC;

		/// <summary>
		/// GPU usada.
		/// </summary>
		VkPhysicalDevice gpu;

		/// <summary>
		/// Informaci�n de la GPU seleccionada.
		/// </summary>
		VULKAN::GPUinfo gpuInfo {};

		/// <summary>
		/// Propiedades de la memoria de la GPU.
		/// </summary>
		VkPhysicalDeviceMemoryProperties memoryProperties;

		/// <summary>
		/// Superficie sobre la cual se renderiza las im�genes.
		/// </summary>
		VkSurfaceKHR surface;

		/// <summary>
		/// Logical device.
		/// </summary>
		VkDevice logicalDevice;

		/// <summary>
		/// Framebuffers.
		/// </summary>
		std::vector<VULKAN::Framebuffer*> framebuffers;

		/// <summary>
		/// Sync.
		/// GPU to CPU.
		/// Imagen lista.
		/// </summary>
		std::vector<VkSemaphore> imageAvailableSemaphores;

		/// <summary>
		/// Sync.
		/// GPU to CPU.
		/// Imagen renderizada.
		/// </summary>
		std::vector<VkSemaphore> renderFinishedSemaphores;

		/// <summary>
		/// Sync.
		/// CPU to GPU.
		/// </summary>
		std::vector<VkFence> inFlightFences;

		/// <summary>
		/// Sync.
		/// CPU to GPU.
		/// </summary>
		std::vector<VkFence> imagesInFlight;

		/// <summary>
		/// Cola de gr�ficos.
		/// </summary>
		VkQueue graphicsQueue;

		/// <summary>
		/// Cola de presentaci�n.
		/// </summary>
		VkQueue presentQueue;

		/// <summary>
		/// Command pool.
		/// </summary>
		VkCommandPool commandPool;

		/// <summary>
		/// Sampler global.
		/// </summary>
		VkSampler globalImageSampler;

		/// <summary>
		/// Nos permite imprimir mensajes de las capas de validaci�n.
		/// </summary>
		VkDebugUtilsMessengerEXT debugConsole;

		struct {
			bool hasFramebufferBeenResized = false;
			uint32_t currentImage = 0;
		} renderVars;

		/// <summary>
		/// True si el renderizador se ha inicializado.
		/// </summary>
		bool hasBeenInit = false;

		/// <summary>
		/// Propiedades de la memoria de la GPU.
		/// </summary>
		VkPhysicalDeviceMemoryProperties memProperties;

		//POST-PROCESSING

		/// <summary>
		/// Graphics para renderizar los post procesamientos.
		/// </summary>
		GraphicsPipeline* screenGraphicsPipeline = nullptr;

		/// <summary>
		/// DescPool para renderizar los post procesamientos.
		/// </summary>
		DescriptorPool* screenDescriptorPool = nullptr;

		/// <summary>
		/// DescLayout para renderizar los post procesamientos.
		/// </summary>
		DescriptorLayout* screenDescriptorLayout = nullptr;

		/// <summary>
		/// DescSet para renderizar los post procesamientos.
		/// </summary>
		DescriptorSet* screenDescriptorSet = nullptr;

		/// <summary>
		/// Renderpass para renderizar los post procesamientos.
		/// </summary>
		VULKAN::Renderpass* screenRenderpass = nullptr;

		/// <summary>
		/// Render target final.
		/// </summary>
		RenderTarget* finalRenderTarget = nullptr;

		/// <summary>
		/// Crea los elementos de post procesamiento.
		/// </summary>
		void InitPostProcessing();

		/// <summary>
		/// Recrea los elementos de post procesamiento.
		/// </summary>
		void RecreatePostProcessing();

		/// <summary>
		/// Cierra los elementos de post procesamiento.
		/// </summary>
		void ClosePostProcessing();


		/// <summary>
		/// Sync.
		/// </summary>
		VkFence* fences = nullptr;

	};

}