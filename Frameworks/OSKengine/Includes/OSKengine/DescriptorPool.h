#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

#include "DynamicArray.hpp"

namespace OSK {

	class DescriptorLayout;

	/// <summary>
	/// Descriptor pool: 
	/// clase mediante la cual se crean descriptor sets.
	/// Encapsula el descriptor pool nativo de Vulkan.
	/// </summary>
	class OSKAPI_CALL DescriptorPool {

		friend class RenderAPI;
		friend class DescriptorSet;

	public:

		/// <summary>
		/// Crea un descriptor pool.
		/// </summary>
		/// <param name="logicalDevice">Logical device del renderizador.</param>
		/// <param name="shapchainImageCount">Número de imágenes del swapchain del renderizador.</param>
		DescriptorPool(VkDevice logicalDevice, uint32_t shapchainImageCount);

		/// <summary>
		/// Destruye el descriptor pool (y por tanto, sus descriptor sets).
		/// </summary>
		~DescriptorPool();

		/// <summary>
		/// Añade un binding.
		/// Se usa internamente al llamar a SetLayout.
		/// </summary>
		/// <param name="type"></param>
		void AddBinding(VkDescriptorType type);

		/// <summary>
		/// Establece el layout que va a tener el descriptor pool.
		/// </summary>
		/// <param name="layout">Descriptor layout.</param>
		void SetLayout(DescriptorLayout* layout);

		/// <summary>
		/// Crea el descriptor pool con tamaño para los sets dados.
		/// </summary>
		/// <param name="maxSets">Tamaño del pool.</param>
		void Create(uint32_t maxSets);
		
	private:

		/// <summary>
		/// Información del layout.
		/// </summary>
		DynamicArray<VkDescriptorPoolSize> descriptorPoolSizes;

		/// <summary>
		/// Descriptor pool nativo de Vulkan.
		/// </summary>
		VkDescriptorPool vulkanDescriptorPool = VK_NULL_HANDLE;

		/// <summary>
		/// Número de imágenes del swapchain del renderizador.
		/// </summary>
		uint32_t swapchainImageCount = 0;

		/// <summary>
		/// Número de imágenes del swapchain del renderizador.
		/// </summary>
		VkDevice logicalDevice = VK_NULL_HANDLE;

	};

}