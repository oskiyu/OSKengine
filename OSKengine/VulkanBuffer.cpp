#include "VulkanBuffer.h"

#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"

#include "Log.h"

#include <stdexcept>

namespace OSK::VULKAN {

	void VulkanBuffer::CreateBuffer(const VulkanDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
		//Struct con la información sore el buffer
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; //Tipo de struct
		bufferInfo.size = size; //Tamaño del buffer
		bufferInfo.usage = usage; //Uso que se le va a dar al buffer
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		//Crear el buffer
		VkResult result = vkCreateBuffer(device.LogicalDevice, &bufferInfo, NULL, &Buffer);

		//Error handling
		if (result != VK_SUCCESS) {
			throw std::runtime_error("ERROR: crear buffer");
		}

		//Asignar memoria
		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(device.LogicalDevice, Buffer, &memoryRequirements);

		//struct con la información de la memoria que se va a usar
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO; //Tipo de struct
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = device.FindMemoryType(memoryRequirements.memoryTypeBits, properties);

		//Asignar la memroia
		result = vkAllocateMemory(device.LogicalDevice, &allocInfo, NULL, &BufferMemory);

		//Error handling
		Logger::DebugLog(std::to_string(result));
		if (result != VK_SUCCESS) {
			throw std::runtime_error("ERROR: asignar memoria para el buffer");
		}

		//ASociar la memoria con el buffer
		vkBindBufferMemory(device.LogicalDevice, Buffer, BufferMemory, 0);
	}


	void VulkanBuffer::CopyBufferFrom(const VulkanDevice& device, VulkanCommandPool* commandPool, const VulkanBuffer& sourceBuffer, const size_t& size, VkQueue Q) {
		//Crear un buffer temporal
		VulkanCommandBuffer commandBuffer{};
		commandBuffer.BeginSingleTimeCommand(device, commandPool);

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer.CommandBuffer, sourceBuffer.Buffer, Buffer, 1, &copyRegion);

		commandBuffer.EndSingleTimeCommand(device, Q);
	}


	void VulkanBuffer::DestroyBuffer(const VulkanDevice& device) {
		//Cleanup
		vkDestroyBuffer(device.LogicalDevice, Buffer, NULL);
		vkFreeMemory(device.LogicalDevice, BufferMemory, NULL);
	}

}