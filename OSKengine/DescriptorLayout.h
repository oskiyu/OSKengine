#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace OSK {

	//Un descriptor layout describe, a grandes rasgos, c�mo se pasa la informaci�n de la CPU a la GPU.
	class OSKAPI_CALL DescriptorLayout {

		friend class RenderAPI;

	public:

		//Destructor del descriptor layout.
		~DescriptorLayout();

		//Declara un nuevo bloque de informaci�n para la comunicaci�n CPU -> GPU.
		//	<binding>: binding del bloque en los shaders (0, 1, 2...).
		//	<type>: tipo de informac�o que se va a pasar (UBO, imagen, etc).
		//	<stage>: shaders que podr�n acceder a esta informaci�n (vertex, fragment, etc).
		//	<count>: ???.
		void AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stage, uint32_t count = 1);
		
		//Crea el descriptor layout.
		//	<maxSets>: n�mero m�ximo de objetos que podr�n usar este descriptor layout.
		void Create(uint32_t maxSets);

		VkDescriptorSetLayout VulkanDescriptorSetLayout;
		VkDescriptorPool VulkanDescriptorPool;

	private:

		DescriptorLayout(VkDevice logicalDevice, uint32_t swapchainCount);

		void clear();

		std::vector<VkDescriptorSetLayoutBinding> descriptorLayoutBindings{};
		std::vector<VkDescriptorPoolSize> descriptorPoolSizes{};

		uint32_t swapchainImageCount;

		VkDevice logicalDevice;

	};

}