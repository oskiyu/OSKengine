#pragma once

#include <exception>
#include <assert.h>
#include <algorithm>

#include <vulkan/vulkan.h>

#include "VulkanInitializers.hpp"
#include "VulkanTools.h"

#include "QueueFamilyIndices.h"
#include <set>

namespace OSK::VULKAN {

	struct VulkanBuffer;

	struct VulkanInstance;

	struct VulkanSurface;

	struct VulkanDevice {

		VkPhysicalDevice PhysicalDevice;


		VkDevice LogicalDevice;


		void CreateLogicalDevice(const VulkanInstance& instance, VulkanSurface& surface, VkQueue& graphicsQ);


		void Destroy();


		void PickPhysicalDevice(const VulkanInstance& instance, const VulkanSurface& surface);

		//Comprueba si la GPU dada puede ejecutar las operaciones necesarias para renderizar el juego
		bool IsPhysicalDeviceSuitable(const VulkanSurface& surface, VkPhysicalDevice physicalDevice);


		//Devuelve las QueueFamily () disponibles en la GPU 'device'
		QueueFamilyIndices FindQueueFamilies(VulkanSurface surface, VkPhysicalDevice physicalDevice) const;


		//Devuelve la información sobre las capacidades de swapchain de la gráfica
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VulkanSurface surface);


		//Obtiene la memoria de la GPU
		uint32_t FindMemoryType(uint32_t type, VkMemoryPropertyFlags properties) const;


		//Comprobar que la GPU soporta las extensiones de Vulkan necesarias
		bool CheckExtensionSupport(VkPhysicalDevice device);


		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

	};

}
/*
struct VulkanDevice {
	VkPhysicalDevice physicaLDevice;
	VkDevice logicalDevice;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	VkPhysicalDeviceFeatures enabledFeatures;
	VkPhysicalDeviceMemoryProperties memoryProperties;
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;
	std::vector<std::string> supportedExtensions;

	VkCommandPool commandPool = VK_NULL_HANDLE;

	bool enableDebugMarkers = false;

	//Queue family indices
	struct {
		uint32_t graphics;
		uint32_t compute;
		uint32_t transfer;
	} queueFamilyIndices;

	//Cast to vkdevice
	operator VkDevice() {
		return logicalDevice;
	}

	//Constructor
	VulkanDevice(VkPhysicalDevice gpu) {
		physicaLDevice = gpu;

		vkGetPhysicalDeviceProperties(physicaLDevice, &properties);
		vkGetPhysicalDeviceFeatures(physicaLDevice, &features);
		vkGetPhysicalDeviceMemoryProperties(physicaLDevice, &memoryProperties);
		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicaLDevice, &queueFamilyCount, nullptr);
		queueFamilyProperties.resize(queueFamilyCount);

		uint32_t extensionsCount = 0;
		vkEnumerateDeviceExtensionProperties(physicaLDevice, nullptr, &extensionsCount, nullptr);
		if (extensionsCount > 0) {
			std::vector<VkExtensionProperties> extensions(extensionsCount);
			if (vkEnumerateDeviceExtensionProperties(physicaLDevice, nullptr, &extensionsCount, &extensions.front()) == VK_SUCCESS) {
				for (auto ext : extensions) {
					supportedExtensions.push_back(ext.extensionName);
				}
			}
		}
	}

	//Destructor
	~VulkanDevice() {
		if (commandPool)
			vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

		if (logicalDevice)
			vkDestroyDevice(logicalDevice, nullptr);
	}

	uint32_t getMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound = nullptr) {
		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
			if ((typeBits & 1) == 1) {
				if ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
					if (memTypeFound)
						*memTypeFound = true;

					return i;
				}
			}
			typeBits >>= 1;
		}

		if (memTypeFound) {
			*memTypeFound = false;

			return 0;
		}

		return 0; //TODO: throw
	}

	//Get index of a queue family that supports the requested queue flags
	uint32_t getQueueFamilyIndex(VkQueueFlagBits queueFlags) {
		///Compute only
		if (queueFlags & VK_QUEUE_COMPUTE_BIT) {
			for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++) {
				if ((queueFamilyProperties[i].queueFlags & queueFlags) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
					return i;
				}
			}
		}

		//Transfer only
		if (queueFlags & VK_QUEUE_TRANSFER_BIT) {
			for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++) {
				if ((queueFamilyProperties[i].queueFlags & queueFlags) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)) {
					return i;
				}
			}
		}
		for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++) {
			if (queueFamilyProperties[i].queueFlags & queueFlags)
				return i;
		}

		return -1;
	}

	//Creates logical device, gets default queue family indices
	VkResult createLogicalDevice(VkPhysicalDeviceFeatures enabledFeatures, std::vector<const char*> enabledExtensions, void* pNextChain, bool useSwapChain = true, VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT) {
		std::vector<VkDeviceQueueCreateInfo> queuecreateInfos{};
		const float_t defaultQueuePriority(0.0f);

		//Graphics Q
		if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT) {
			queueFamilyIndices.graphics = getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
			VkDeviceQueueCreateInfo qInfo = {};
			qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			qInfo.queueFamilyIndex = queueFamilyIndices.graphics;
			qInfo.queueCount = 1;
			qInfo.pQueuePriorities = &defaultQueuePriority;

			queuecreateInfos.push_back(qInfo);
		}
		else {
			queueFamilyIndices.graphics = VK_NULL_HANDLE;
		}

		//Compute Q
		if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT) {
			queueFamilyIndices.compute = getQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
			if (queueFamilyIndices.compute != queueFamilyIndices.graphics) {
				VkDeviceQueueCreateInfo qInfo = {};
				qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				qInfo.queueFamilyIndex = queueFamilyIndices.compute;
				qInfo.queueCount = 1;
				qInfo.pQueuePriorities = &defaultQueuePriority;

				queuecreateInfos.push_back(qInfo);
			}
		}
		else {
			queueFamilyIndices.compute = queueFamilyIndices.graphics;
		}

		//Transfer Q
		if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT) {
			queueFamilyIndices.transfer = getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);
			if ((queueFamilyIndices.transfer != queueFamilyIndices.graphics) && (queueFamilyIndices.transfer != queueFamilyIndices.compute)) {
				VkDeviceQueueCreateInfo qInfo = {};
				qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				qInfo.queueFamilyIndex = queueFamilyIndices.transfer;
				qInfo.queueCount = 1;
				qInfo.pQueuePriorities = &defaultQueuePriority;

				queuecreateInfos.push_back(qInfo);
			}
		}
		else {
			queueFamilyIndices.transfer = queueFamilyIndices.graphics;
		}

		//Logical deice
		std::vector<const char*> deviceExtensions(enabledExtensions);
		if (useSwapChain) {
			//Request swapchain extension
			deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		}

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queuecreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queuecreateInfos.data();
		deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

		if (pNextChain) {
			VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{};
			physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
			physicalDeviceFeatures2.features = enabledFeatures;
			physicalDeviceFeatures2.pNext = pNextChain;
			deviceCreateInfo.pEnabledFeatures = nullptr;
			deviceCreateInfo.pNext = &physicalDeviceFeatures2;
		}

		//Debug
		if (extensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) {
			deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
			enableDebugMarkers = true;
		}

		if (deviceExtensions.size() > 0) {
			deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
			deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		}

		VkResult result = vkCreateDevice(physicaLDevice, &deviceCreateInfo, nullptr, &logicalDevice);

		if (result == VK_SUCCESS) {
			commandPool = createCommandPool(queueFamilyIndices.graphics);
		}

		this->enabledFeatures = enabledFeatures;

		return result;
	}

	//Crea un buffer
	VkResult createBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, VkBuffer* buffer, VkDeviceMemory* memory, void* data = nullptr) {
		VkBufferCreateInfo bufferCreateInfo = Init::bufferCreateInfo(usageFlags, size);
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vkCreateBuffer(logicalDevice, &bufferCreateInfo, nullptr, buffer);

		VkMemoryRequirements memRequs;
		VkMemoryAllocateInfo mAlloc = Init::memoryAllocateInfo();
		vkGetBufferMemoryRequirements(logicalDevice, *buffer, &memRequs);
		mAlloc.allocationSize = memRequs.size;
		mAlloc.memoryTypeIndex = getMemoryType(memRequs.memoryTypeBits, memoryPropertyFlags);
		vkAllocateMemory(logicalDevice, &mAlloc, nullptr, memory);

		if (data != nullptr) {
			void* mapped;
			vkMapMemory(logicalDevice, *memory, 0, size, 0, &mapped);
			memcpy(mapped, data, size);

			if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) {
				VkMappedMemoryRange mappedRange = Init::mappedMemoryRange();
				mappedRange.memory = *memory;
				mappedRange.offset = 0;
				mappedRange.size = size;

				vkFlushMappedMemoryRanges(logicalDevice, 1, &mappedRange);
			}
			vkUnmapMemory(logicalDevice, *memory);
		}

		vkBindBufferMemory(logicalDevice, *buffer, *memory, 0);

		return VK_SUCCESS;
	}

	VkResult createBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, Buffer* buffer, VkDeviceSize size, void* data = nullptr) {
		buffer->device = logicalDevice;

		VkBufferCreateInfo bufferCreateinfo = Init::bufferCreateInfo(usageFlags, size);
		vkCreateBuffer(logicalDevice, &bufferCreateinfo, nullptr, &buffer->buffer);

		VkMemoryRequirements memRequs;
		VkMemoryAllocateInfo mAlloc = Init::memoryAllocateInfo();
		mAlloc.allocationSize = memRequs.size;
		mAlloc.memoryTypeIndex = getMemoryType(memRequs.memoryTypeBits, memoryPropertyFlags);
		vkAllocateMemory(logicalDevice, &mAlloc, nullptr, &buffer->memory);

		buffer->alignment = memRequs.alignment;
		buffer->size = size;
		buffer->usageFlags = usageFlags;
		buffer->memoryPropertyFlags = memoryPropertyFlags;

		if (data != nullptr) {
			buffer->map();
			memcpy(buffer->mapped, data, size);
			if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
				buffer->flush();

			buffer->unmap();
		}

		buffer->setupDescriptor();

		return buffer->bind();
	}

	void copyBuffer(Buffer* source, Buffer* destiny, VkQueue Q, VkBufferCopy* copyRegion = nullptr) {
		//assert(dst.size <= src.size
		//assert(src.buffer
		VkCommandBuffer copyCmd = createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
		VkBufferCopy bufferCopy = {};
		if (copyRegion == nullptr) {
			bufferCopy.size = source->size;
		}
		else {
			bufferCopy = *copyRegion;
		}

		vkCmdCopyBuffer(copyCmd, source->buffer, destiny->buffer, 1, &bufferCopy);

		flushCommandBuffer(copyCmd, Q);
	}

	VkCommandPool createCommandPool(uint32_t qFamilyIndex, VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) {
		VkCommandPoolCreateInfo cmdPoolInfo = {};
		cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolInfo.queueFamilyIndex = qFamilyIndex;
		cmdPoolInfo.flags = createFlags;

		VkCommandPool cmdpool;
		vkCreateCommandPool(logicalDevice, &cmdPoolInfo, nullptr, &cmdpool);

		return cmdpool;
	}

	VkCommandBuffer createCommandBuffer(VkCommandBufferLevel level, VkCommandPool pool, bool begin = false) {
		VkCommandBufferAllocateInfo cmdBufAllocInfo = Init::commandBufferAllocateInfo(pool, level, 1);

		VkCommandBuffer cmdBuffer;
		vkAllocateCommandBuffers(logicalDevice, &cmdBufAllocInfo, &cmdBuffer);
		if (begin) {
			VkCommandBufferBeginInfo cmdBufInfo = Init::commandBufferBeginInfo();
			vkBeginCommandBuffer(cmdBuffer, &cmdBufInfo);
		}

		return cmdBuffer;
	}

	VkCommandBuffer createCommandBuffer(VkCommandBufferLevel level, bool begin = false) {
		return createCommandBuffer(level, commandPool, begin);
	}

	//Submit to a queue
	void flushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue Q, VkCommandPool pool, bool free = true) {
		if (commandBuffer == VK_NULL_HANDLE)
			return;

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = Init::submitInfo();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		//Make sure command buffer has finished executing
		VkFenceCreateInfo fenceInfo = Init::fenceCreateInfo(VK_FLAGS_NONE);//VK_FLAGS_NONE
		VkFence fence;
		vkCreateFence(logicalDevice, &fenceInfo, nullptr, &fence);
		vkQueueSubmit(Q, 1, &submitInfo, fence);
		vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);

		if (free)
			vkFreeCommandBuffers(logicalDevice, pool, 1, &commandBuffer);
	}

	void flushCommandBuffer(VkCommandBuffer cmdBuffer, VkQueue Q, bool free = true) {
		flushCommandBuffer(cmdBuffer, Q, commandPool, free);
	}

	bool extensionSupported(std::string extension) {
		return std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.end();
	}

	VkFormat getSupportedDepthFormat(bool checkSamplingSupport) {
		const std::vector<VkFormat> depthFormats = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM
		};

		for (auto& format : depthFormats) {
			VkFormatProperties formatProperties;
			vkGetPhysicalDeviceFormatProperties(physicaLDevice, format, &formatProperties);
			if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
				if (checkSamplingSupport) {
					if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)) {
						continue;
					}
				}
				return format;
			}
		}

		return VK_FORMAT_D16_UNORM;
	}

};
*/