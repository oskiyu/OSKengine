#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include <vulkan/vulkan.h>

#include "IGpu.h"
#include "QueueFamilyIndices.h"
#include "DynamicArray.hpp"

namespace OSK::GRAPHICS {

	/// @brief Implementaci�n de la interfaz para el renderizador de Vulkan.
	class OSKAPI_CALL GpuVk final : public IGpu {

	public:
				
		/// @brief Informaci�n de las capacidades y caracter�sticas de
		/// una GPU en concreto.
		struct Info {

			/// @brief Detalles sobre el soporte de swapchain.
			struct SwapchainSupportDetails {
								
				/// @brief Capacidades de la superficie. 
				/// Incluye:
				/// - N�mero m�nimo y m�ximo de im�genes.
				/// - Tama�o m�nimo y m�ximo de im�genes.
				/// - Usos soportados de las im�genes.
				VkSurfaceCapabilitiesKHR surfaceCapabilities{};

				/// @brief Formatos de im�gen soportados.
				/// Incluye formatos y espacios de color.
				DynamicArray<VkSurfaceFormatKHR> formats;

				/// @brief Modos de presentaci�n soportados.
				DynamicArray<VkPresentModeKHR> presentModes;

			};


			/// @brief Propiedades b�sicas de la GPU.
			/// Incluye:
			/// - Informaci�n sobre los drivers.
			/// - Nombre de la tarjeta y fabricante.
			/// - L�mites, incluyendo tamalo m�ximo de imagen soportado,
			/// tama�o m�ximo de buffer que se puede crear, n�mero m�ximo de
			/// descriptor sets, etc...
			VkPhysicalDeviceProperties properties{};

			/// @brief Informaci�n sobre el soporte de caracter�sticas b�sicas.
			VkPhysicalDeviceFeatures features{};
			
			/// @brief M�ximo nivel de MSAA disponible.
			VkSampleCountFlagBits maxMsaaSamples{};

			/// @brief Informaci�n de las heap de memoria disponibles en la GPU.
			/// Incluye tipo, propiedades, tama�o, etc...
			VkPhysicalDeviceMemoryProperties memoryProperties{};


			/// @brief Estructura que contiene las propiedades b�sicas de la GPU.
			/// Tambi�n funciona como el primer nodo de una lista enlazada con
			/// m�s propiedades espec�ficas de la GPU.
			VkPhysicalDeviceFeatures2 extendedFeatures{};

			/// @brief Propiedades y l�mites de los pipelines de trazado de rayos.
			VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtPipelineProperties{};
			/// @brief Caracter�sticas soportadas de trazado derayos.
			VkPhysicalDeviceRayTracingPipelineFeaturesKHR rtPipelineFeatures{};

			/// @brief Caracter�sticas soportadas de las estructuras de aceleraci�n.
			VkPhysicalDeviceAccelerationStructureFeaturesKHR rtAccelerationStructuresFeatures{};
			/// @brief Propiedades soportadas de las estructuras de aceleraci�n.
			VkPhysicalDeviceAccelerationStructurePropertiesKHR rtAccelerationStructuresProperites{};

			/// @brief Propiedades sobre la capacidad para obtener la direcci�n de
			/// memoria de un buffer en la GPU.
			VkPhysicalDeviceBufferDeviceAddressFeatures rtDeviceAddressFeatures{};
			
			/// @brief Caracter�sticas soportadas en descriptor sets.
			/// Incluye poder actualizar un descriptor set mientras se usa y
			/// tener descriptor sets de un tama�o indeterminado.
			VkPhysicalDeviceDescriptorIndexingFeatures bindlessTexturesSets{};

			/// @brief Caracter�sticas soportadas para mesh shaders.
			VkPhysicalDeviceMeshShaderFeaturesEXT meshShaders{};

			/// @brief Contiene informaci�n sobre si soporta el renderizado sin usar
			/// render passes.
			VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};

			/// @brief Contiene informaci�n sobre el soporte de sincronizaci�n avanzada.
			VkPhysicalDeviceSynchronization2Features syncFeatures{};

			/// @brief Comprueba si esta GPU puede renderizar trazado de rayos.
			/// @return True si soporta trazado de rayos.
			bool IsRtCompatible() const;

			bool IsCompatibleWithBindless() const;
			bool IsCompatibleWithMeshShaders() const;


			/// @brief Alineamiento m�nimo para dynamic ubos.
			USize64 minAlignment = 0;

			/// @brief True si puede usarse en OSKengine.
			bool isSuitable = false;

			/// @brief Detalles del swapchain.
			SwapchainSupportDetails swapchainSupportDetails{};


			/// @brief Obtiene la informaci�n relevante de una GPU en concreto.
			/// @param gpu Handler de la GPU.
			/// @param surface Superficie sobre la que se va a renderizar.
			/// @return Informaci�n.
			static Info Get(VkPhysicalDevice gpu, VkSurfaceKHR surface);


			/// @brief Handler de la GPU.
			VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		};

		GpuVk(VkPhysicalDevice gpu, VkSurfaceKHR surface);
		~GpuVk() override;

		GpuMemoryUsageInfo GetMemoryUsageInfo() const override;

		/// @brief Devuelve la informaci�n de esta GPU.
		/// @return Informaci�n de esta GPU.
		const Info& GetInfo() const;

		/// @brief Obtiene el handler de Vulkan de la GPU.
		/// @return Handler de la GPU.
		VkPhysicalDevice GetPhysicalDevice() const;

		/// @brief Crea el logical device de esta GPU.
		/// Necesario si se va a usar esta GPU.
		/// @throws LogicalDeviceCreationException Si ocurre alg�n error.
		void CreateLogicalDevice();

		/// @brief Devuelve el logical device asignado a esta GPU.
		/// @return Logical device de la GPU.
		/// 
		/// @pre Se debe haber llamado a GpuVulkan::CreateLogicalDevice.
		VkDevice GetLogicalDevice() const;

		/// @brief Devuelve la superficie sobre la que renderiza esta GPU.
		/// @return Superficie sobre la que renderiza esta GPU.
		VkSurfaceKHR GetSurface() const;

		/// @brief Devuelve informaci�n sobre las colas soportadas por esta GPU.
		/// @return Colas de comandos soportadas por esta GPU.
		QueueFamiles GetQueueFamilyIndices() const;


		UniquePtr<ICommandPool> CreateGraphicsCommandPool() override;
		UniquePtr<ICommandPool> CreateComputeCommandPool() override;
		std::optional<UniquePtr<ICommandPool>> CreateTransferOnlyCommandPool() override;
		UniquePtr<IGpuImageSampler> CreateSampler(const GpuImageSamplerDesc& info) const override;

	private:

		/// @brief Elimina el logical device.
		void Close() override;

		/// @brief Comrpueba si una extensi�n se encuentra en la lista dada.
		/// @param name Nombre de la extensi�n de Vulkan.
		/// @param extensions Extensiones soportadas.
		/// @return True si la extensi�n est� soportada.
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
