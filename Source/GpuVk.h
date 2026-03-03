#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include <vulkan/vulkan.h>

#include "IGpu.h"
#include "QueueFamilyIndices.h"
#include "DynamicArray.hpp"
#include "GpuInfoVkAny.h"
#include "VulkanTarget.h"

namespace OSK::GRAPHICS {

	template <VulkanTarget Target>
	class OSKAPI_CALL GpuVk final : public IGpu {

	public:

		GpuVk(VkPhysicalDevice gpu, VkSurfaceKHR surface);
		~GpuVk() override;

		GpuMemoryUsageInfo GetMemoryUsageInfo() const override;

		/// @brief Devuelve la información de esta GPU.
		/// @return Información de esta GPU.
		const GpuInfoVkAny<Target>& GetInfo() const;

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


		/// @brief Comrpueba si una extensión se encuentra en la lista dada.
		/// @param name Nombre de la extensión de Vulkan.
		/// @param extensions Extensiones soportadas.
		/// @return True si la extensión está soportada.
		static bool IsExtensionPresent(const char* name, const DynamicArray<VkExtensionProperties>& extensions);

		/// @brief Devuelve las extensiones soportadas por una GPU.
		/// @param gpu GPU comprobada.
		/// @return Lista con todas las extensiones soportadas.
		static DynamicArray<VkExtensionProperties> GetAvailableExtensions(VkPhysicalDevice gpu);

	private:

		/// @brief Elimina el logical device.
		void Close() override;

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice logicalDevice = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		GpuInfoVkAny<Target> info{};

	};

	template class GpuVk<VulkanTarget::VK_1_0>;
	template class GpuVk<VulkanTarget::VK_LATEST>;

}

#endif
