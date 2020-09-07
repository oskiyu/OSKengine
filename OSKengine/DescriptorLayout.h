#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace OSK {

	//Un descriptor layout describe, a grandes rasgos, cómo se pasa la información de la CPU a la GPU.
	class DescriptorLayout {

		friend class VulkanRenderer;

	public:

		//Destructor del descriptor layout.
		~DescriptorLayout();

		//Declara un nuevo bloque de información para la comunicación CPU -> GPU.
		//	<binding>: binding del bloque en los shaders (0, 1, 2...).
		//	<type>: tipo de informacío que se va a pasar (UBO, imagen, etc).
		//	<stage>: shaders que podrán acceder a esta información (vertex, fragment, etc).
		//	<count>: ???.
		void AddBinding(const uint32_t& binding, VkDescriptorType type, VkShaderStageFlags stage, const uint32_t& count = 1);
		
		//Crea el descriptor layout.
		//	<maxSets>: número máximo de objetos que podrán usar este descriptor layout.
		void Create(const uint32_t& maxSets);

		VkDescriptorSetLayout VulkanDescriptorSetLayout;
		VkDescriptorPool VulkanDescriptorPool;

	private:

		DescriptorLayout(VkDevice logicalDevice, const uint32_t& swapchainCount);

		void clear();

		std::vector<VkDescriptorSetLayoutBinding> descriptorLayoutBindings{};
		std::vector<VkDescriptorPoolSize> descriptorPoolSizes{};

		uint32_t swapchainImageCount;

		VkDevice logicalDevice;

	};

}