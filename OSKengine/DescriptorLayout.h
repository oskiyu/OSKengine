#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>
#include <vector>

#include "DynamicArray.hpp"

namespace OSK {

	class DescriptorPool;

	//Un descriptor layout describe, a grandes rasgos, cómo se pasa la información de la CPU a la GPU.
	class OSKAPI_CALL DescriptorLayout {

		friend class RenderAPI;
		friend class DescriptorPool;

	public:

		DescriptorLayout(VkDevice logicalDevice) : LogicalDevice(logicalDevice) {

		}

		//Destructor del descriptor layout.
		~DescriptorLayout();

		//Declara un nuevo bloque de información para la comunicación CPU -> GPU.
		//	<binding>: binding del bloque en los shaders (0, 1, 2...).
		//	<type>: tipo de informacío que se va a pasar (UBO, imagen, etc).
		//	<stage>: shaders que podrán acceder a esta información (vertex, fragment, etc).
		//	<count>: ???.
		void AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage, uint32_t count = 1);
		
		void AddBinding(VkDescriptorType type, VkShaderStageFlags stage);

		//Crea el descriptor layout.
		void Create();

		VkDescriptorSetLayout VulkanDescriptorSetLayout = VK_NULL_HANDLE;
		
		DescriptorPool* DPool = nullptr;

		DynamicArray<VkDescriptorSetLayoutBinding> DescriptorLayoutBindings{};

		VkDevice LogicalDevice;

	};

}
