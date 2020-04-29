#include "VulkanCommandPool.h"

#include "VulkanDevice.h"
#include "VulkanSurface.h"

namespace OSK::VULKAN {

	void VulkanCommandPool::CreateCommandPool(const VulkanDevice& device, const VulkanSurface& surface) {
		//Fila en la que irán los comandos
		QueueFamilyIndices queueFamilyIndices = device.FindQueueFamilies(surface, device.PhysicalDevice);

		//struct con la información del command pool
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO; //Tipo de struct
		poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value(); //Fila
		poolInfo.flags = 0; // Optional

		//Crear la command pool
		VkResult result = vkCreateCommandPool(device.LogicalDevice, &poolInfo, NULL, &CommandPool);

		//Error handling
		if (result != VK_SUCCESS) {
			OSK::Logger::Log(OSK::LogMessageLevels::CRITICAL_ERROR, "crear command pool", __LINE__);
			throw std::runtime_error("ERROR: crear command pool");
		}
	}


	void VulkanCommandPool::DestroyCommandPool(const VulkanDevice& device) {
		//Destruir la command pool
		vkDestroyCommandPool(device.LogicalDevice, CommandPool, NULL);
	}

}