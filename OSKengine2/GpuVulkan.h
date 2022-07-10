#pragma once

#include <vulkan/vulkan.h>

#include "IGpu.h"
#include "QueueFamilyIndices.h"
#include "DynamicArray.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Implementaci�n de la interfaz para el renderizador de Vulkan.
	/// </summary>
	class OSKAPI_CALL GpuVulkan : public IGpu {

	public:

		/// <summary>
		/// Informaci�n interna de una GPU.
		/// </summary>
		struct Info {

			/// <summary>
			/// Detalles sobre el soporte de swapchain de una GPU.
			/// </summary>
			struct SwapchainSupportDetails {

				/// <summary>
				/// Capacidades de la superficie.
				/// </summary>
				VkSurfaceCapabilitiesKHR surfaceCapabilities{};

				/// <summary>
				/// Formatos soportados.
				/// </summary>
				DynamicArray<VkSurfaceFormatKHR> formats;

				/// <summary>
				/// Modos de presentaci�n soportados.
				/// </summary>
				DynamicArray<VkPresentModeKHR> presentModes;

			};

			/// <summary>
			/// Propiedades f�sicas.
			/// </summary>
			VkPhysicalDeviceProperties properties{};

			/// <summary>
			/// Cosas que soporta.
			/// </summary>
			VkPhysicalDeviceFeatures features{};

			/// <summary>
			/// M�ximo nivel de MSAA disponible.
			/// </summary>
			VkSampleCountFlagBits maxMsaaSamples{};

			/// <summary>
			/// Informaci�n de las memorias disponibles en la GPU.
			/// </summary>
			VkPhysicalDeviceMemoryProperties memoryProperties{};

			VkPhysicalDeviceFeatures2 features2{};
			VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtPipelineProperties{};
			VkPhysicalDeviceRayTracingPipelineFeaturesKHR rtPipelineFeatures{};
			VkPhysicalDeviceAccelerationStructureFeaturesKHR rtAccelerationStructuresFeatures{};
			VkPhysicalDeviceAccelerationStructurePropertiesKHR rtAccelerationStructuresProperites{};
			VkPhysicalDeviceBufferDeviceAddressFeatures rtDeviceAddressFeatures{};

			VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};

			bool IsRtCompatible() const;

			/// <summary>
			/// Alineamiento m�nimo para dynamic ubos.
			/// </summary>
			TSize minAlignment = 0;

			/// <summary>
			/// True si puede usarse en OSKengine.
			/// </summary>
			bool isSuitable = false;

			/// <summary>
			/// Detalles del swapchain.
			/// </summary>
			SwapchainSupportDetails swapchainSupportDetails{};

			/// <summary>
			/// Obtiene la informaci�n relevante de una GPU en concreto.
			/// </summary>
			static Info Get(VkPhysicalDevice gpu, VkSurfaceKHR surface);

		};

		~GpuVulkan();

		/// <summary>
		/// Elimina el logical device.
		/// </summary>
		void Close() override;

		OwnedPtr<ICommandPool> CreateCommandPool() override;
		OwnedPtr<ISyncDevice> CreateSyncDevice() override;

		void SetPhysicalDevice(VkPhysicalDevice gpu);
		VkPhysicalDevice GetPhysicalDevice() const;

		/// <summary>
		/// Actualiza la informaci�n conocida sobre esta gr�fica.
		/// </summary>
		void SetInfo(const Info& info);
		const Info& GetInfo() const;

		void CreateLogicalDevice(VkSurfaceKHR surface);
		VkDevice GetLogicalDevice() const;

		VkSurfaceKHR GetSurface() const;

		/// <summary>
		/// Devuelve informaci�n sobre las colas soportadas por esta GPU.
		/// </summary>
		QueueFamilyIndices GetQueueFamilyIndices(VkSurfaceKHR surface) const;

	private:

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice logicalDevice = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		Info info{};

	};

}
