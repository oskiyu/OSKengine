#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include <vulkan/vulkan.h>

#include "IGpu.h"
#include "QueueFamilyIndices.h"
#include "DynamicArray.hpp"

namespace OSK::GRAPHICS {

	/// @brief Implementación de la interfaz para el renderizador de Vulkan.
	class OSKAPI_CALL GpuVk final : public IGpu {

	public:
				
		/// @brief Información de las capacidades y características de
		/// una GPU en concreto.
		struct Info {

			/// @brief Detalles sobre el soporte de swapchain.
			struct SwapchainSupportDetails {
								
				/// @brief Capacidades de la superficie. 
				/// Incluye:
				/// - Número mínimo y máximo de imágenes.
				/// - Tamaño mínimo y máximo de imágenes.
				/// - Usos soportados de las imágenes.
				VkSurfaceCapabilitiesKHR surfaceCapabilities{};

				/// @brief Formatos de imágen soportados.
				/// Incluye formatos y espacios de color.
				DynamicArray<VkSurfaceFormatKHR> formats;

				/// @brief Modos de presentación soportados.
				DynamicArray<VkPresentModeKHR> presentModes;

			};


			/// @brief Propiedades básicas de la GPU.
			/// Incluye:
			/// - Información sobre los drivers.
			/// - Nombre de la tarjeta y fabricante.
			/// - Límites, incluyendo tamalo máximo de imagen soportado,
			/// tamaño máximo de buffer que se puede crear, número máximo de
			/// descriptor sets, etc...
			VkPhysicalDeviceProperties properties{};

			/// @brief Información sobre el soporte de características básicas.
			VkPhysicalDeviceFeatures features{};
			
			/// @brief Máximo nivel de MSAA disponible.
			VkSampleCountFlagBits maxMsaaSamples{};

			/// @brief Información de las heap de memoria disponibles en la GPU.
			/// Incluye tipo, propiedades, tamaño, etc...
			VkPhysicalDeviceMemoryProperties memoryProperties{};


			/// @brief Estructura que contiene las propiedades básicas de la GPU.
			/// También funciona como el primer nodo de una lista enlazada con
			/// más propiedades específicas de la GPU.
			VkPhysicalDeviceFeatures2 extendedFeatures{};

			/// @brief Propiedades y límites de los pipelines de trazado de rayos.
			VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtPipelineProperties{};
			/// @brief Características soportadas de trazado derayos.
			VkPhysicalDeviceRayTracingPipelineFeaturesKHR rtPipelineFeatures{};

			/// @brief Características soportadas de las estructuras de aceleración.
			VkPhysicalDeviceAccelerationStructureFeaturesKHR rtAccelerationStructuresFeatures{};
			/// @brief Propiedades soportadas de las estructuras de aceleración.
			VkPhysicalDeviceAccelerationStructurePropertiesKHR rtAccelerationStructuresProperites{};

			/// @brief Propiedades sobre la capacidad para obtener la dirección de
			/// memoria de un buffer en la GPU.
			VkPhysicalDeviceBufferDeviceAddressFeatures rtDeviceAddressFeatures{};
			
			/// @brief Características soportadas en descriptor sets.
			/// Incluye poder actualizar un descriptor set mientras se usa y
			/// tener descriptor sets de un tamaño indeterminado.
			VkPhysicalDeviceDescriptorIndexingFeatures bindlessTexturesSets{};

			/// @brief Características soportadas para mesh shaders.
			VkPhysicalDeviceMeshShaderFeaturesEXT meshShaders{};

			/// @brief Contiene información sobre si soporta el renderizado sin usar
			/// render passes.
			VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};

			/// @brief Contiene información sobre el soporte de sincronización avanzada.
			VkPhysicalDeviceSynchronization2Features syncFeatures{};

			/// @brief Comprueba si esta GPU puede renderizar trazado de rayos.
			/// @return True si soporta trazado de rayos.
			bool IsRtCompatible() const;

			bool IsCompatibleWithBindless() const;
			bool IsCompatibleWithMeshShaders() const;


			/// @brief Alineamiento mínimo para dynamic ubos.
			USize64 minAlignment = 0;

			/// @brief True si puede usarse en OSKengine.
			bool isSuitable = false;

			/// @brief Detalles del swapchain.
			SwapchainSupportDetails swapchainSupportDetails{};


			/// @brief Obtiene la información relevante de una GPU en concreto.
			/// @param gpu Handler de la GPU.
			/// @param surface Superficie sobre la que se va a renderizar.
			/// @return Información.
			static Info Get(VkPhysicalDevice gpu, VkSurfaceKHR surface);


			/// @brief Handler de la GPU.
			VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		};

		GpuVk(VkPhysicalDevice gpu, VkSurfaceKHR surface);
		~GpuVk() override;

		GpuMemoryUsageInfo GetMemoryUsageInfo() const override;

		/// @brief Devuelve la información de esta GPU.
		/// @return Información de esta GPU.
		const Info& GetInfo() const;

		/// @brief Obtiene el handler de Vulkan de la GPU.
		/// @return Handler de la GPU.
		VkPhysicalDevice GetPhysicalDevice() const;

		/// @brief Crea el logical device de esta GPU.
		/// Necesario si se va a usar esta GPU.
		/// @throws LogicalDeviceCreationException Si ocurre algún error.
		void CreateLogicalDevice();

		/// @brief Devuelve el logical device asignado a esta GPU.
		/// @return Logical device de la GPU.
		/// 
		/// @pre Se debe haber llamado a GpuVulkan::CreateLogicalDevice.
		VkDevice GetLogicalDevice() const;

		/// @brief Devuelve la superficie sobre la que renderiza esta GPU.
		/// @return Superficie sobre la que renderiza esta GPU.
		VkSurfaceKHR GetSurface() const;

		/// @brief Devuelve información sobre las colas soportadas por esta GPU.
		/// @return Colas de comandos soportadas por esta GPU.
		QueueFamiles GetQueueFamilyIndices() const;


		UniquePtr<ICommandPool> CreateGraphicsCommandPool() override;
		UniquePtr<ICommandPool> CreateComputeCommandPool() override;
		std::optional<UniquePtr<ICommandPool>> CreateTransferOnlyCommandPool() override;
		UniquePtr<IGpuImageSampler> CreateSampler(const GpuImageSamplerDesc& info) const override;

	private:

		/// @brief Elimina el logical device.
		void Close() override;

		/// @brief Comrpueba si una extensión se encuentra en la lista dada.
		/// @param name Nombre de la extensión de Vulkan.
		/// @param extensions Extensiones soportadas.
		/// @return True si la extensión está soportada.
		static bool IsExtensionPresent(const char* name, const DynamicArray<VkExtensionProperties>& extensions);

		/// @brief Devuelve las extensiones soportadas por una GPU.
		/// @param gpu GPU comprobada.
		/// @return Lista con todas las extensiones soportadas.
		static DynamicArray<VkExtensionProperties> GetAvailableExtensions(VkPhysicalDevice gpu);

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice logicalDevice = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		Info info{};

	};

}

#endif
