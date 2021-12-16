#pragma once

#include "IRenderer.h"
#include "UniquePtr.hpp"

struct VkInstance_T;
typedef VkInstance_T* VkInstance;
struct VkSurfaceKHR_T;
typedef VkSurfaceKHR_T* VkSurfaceKHR;
struct VkDebugUtilsMessengerEXT_T;
typedef VkDebugUtilsMessengerEXT_T* VkDebugUtilsMessengerEXT;

namespace OSK {

	class IRenderpass;

	/// <summary>
	/// Implementación de la interfaz para el renderizador de Vulkan.
	/// </summary>
	class OSKAPI_CALL RendererVulkan : public IRenderer {

	public:

		~RendererVulkan();

		void Initialize(const std::string& appName, const Version& version, const Window& window) override;
		void Close() override;
		void PresentFrame() override;

	protected:

		void CreateCommandQueues() override;
		void CreateSwapchain() override;
		void CreateSyncDevice() override;
		void CreateGpuMemoryAllocator() override;

	private:

		void CreateInstance(const std::string& appName, const Version& version);
		void SetupDebugLogging();
		void CreateSurface(const Window& window);
		void ChooseGpu();

		bool AreValidationLayersAvailable() const;

		VkInstance instance;
		VkSurfaceKHR surface;
		VkDebugUtilsMessengerEXT debugConsole;

		UniquePtr<IRenderpass> renderpass;

	};

}
