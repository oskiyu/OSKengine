#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace OSK {

	class DescriptorLayout {

		friend class VulkanRenderer;

	public:

		~DescriptorLayout();

		void AddBinding(const uint32_t& binding, VkDescriptorType type, VkShaderStageFlags stage, const uint32_t& count = 1);
		
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