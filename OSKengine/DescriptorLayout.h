#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>

#include "DynamicArray.hpp"

#include "MaterialBinding.h"

namespace OSK {

	class DescriptorPool;

	/// <summary>
	/// Un descriptor layout describe, a grandes rasgos, c�mo se pasa la informaci�n de la CPU a la GPU.
	/// Establece cuales son los bindings.
	/// Encapsula el descriptor layout nativo de Vulkan.
	/// </summary>
	class OSKAPI_CALL DescriptorLayout {

		friend class RenderAPI;
		friend class DescriptorPool;
		friend class MaterialSystem;
		friend class DescriptorSet;

	public:

		/// <summary>
		/// Crea un descriptor layout vac�o.
		/// </summary>
		/// <param name="logicalDevice">Logical device del renderizador.</param>
		DescriptorLayout(VkDevice logicalDevice);

		/// <summary>
		/// Destruye el descriptor layout..
		/// </summary>
		~DescriptorLayout();

		/// <summary>
		/// A�ade un nuveo binding.
		/// </summary>
		/// <param name="binding">Binding del bloque en los shaders (0, 1, 2...).</param>
		/// <param name="type">Tipo de informac�o que se va a pasar (UBO, imagen, etc).</param>
		/// <param name="stage">Shaders que podr�n acceder a esta informaci�n (vertex, fragment, etc).</param>
		/// <param name="count">???</param>
		void AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage, uint32_t count = 1);
		
		/// <summary>
		/// A�ade un nuveo binding.
		/// Su n�mero ser� el siguiente no asignado.
		/// </summary>
		/// <param name="type">Tipo de informac�o que se va a pasar (UBO, imagen, etc).</param>
		/// <param name="stage">Shaders que podr�n acceder a esta informaci�n (vertex, fragment, etc).</param>
		void AddBinding(VkDescriptorType type, VkShaderStageFlags stage);

		void AddBinding(OSK::MaterialBindingType type, OSK::MaterialBindingShaderStage stage, const std::string& key);

		uint32_t GetBindingFromName(const std::string& name);

		/// <summary>
		/// Crea el descriptor layout nativo de Vulkan..
		/// </summary>
		void Create();

	private:

		/// <summary>
		/// Descriptor layout nativo de Vulkan.
		/// </summary>
		VkDescriptorSetLayout vulkanDescriptorSetLayout = VK_NULL_HANDLE;
		
		/// <summary>
		/// Lista de bindings del layout.
		/// </summary>
		DynamicArray<VkDescriptorSetLayoutBinding> descriptorLayoutBindings{};

		/// <summary>
		/// Logical device del renderizador.
		/// </summary>
		VkDevice logicalDevice = VK_NULL_HANDLE;

		std::unordered_map<std::string, uint32_t> bindingNames;
		
	};

}
