#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "ICommandPool.h"
#include "NumericTypes.h"
#include "VulkanTypedefs.h"
#include "VulkanTarget.h"

OSK_VULKAN_TYPEDEF(VkCommandPool);
OSK_VULKAN_TYPEDEF(VkDevice);

namespace OSK::GRAPHICS {

	struct QueueFamily;
	template <VulkanTarget> class GpuVk;

	template <VulkanTarget Target>
	class OSKAPI_CALL CommandPoolVk final : public ICommandPool {

	public:

		/// @brief Crea el command pool.
		/// @param device Gpu sobre la que se crea.
		/// @param supportedCommands Tipos de comandos que soportará el pool.
		/// @param queueType Familia de colas sobre la que se enviarán
		/// las listas generadas por este pool.
		/// @param type Tipo de cola.
		/// 
		/// @throws CommandPoolCreationException
		CommandPoolVk(
			const GpuVk<Target>& device,
			const QueueFamily& queueType,
			GpuQueueType type);

		~CommandPoolVk() override;

		UniquePtr<ICommandList> CreateCommandList(const IGpu& device) override;
		UniquePtr<ICommandList> CreateSingleTimeCommandList(const IGpu& device) override;
		
		VkCommandPool GetCommandPool() const;

	private:

		/// @throws CommandListCreationException si no se pudo crear la lista. 
		UniquePtr<ICommandList> CreateList(const IGpu& device, USize32 numNativeLists);

		VkCommandPool m_commandPool = nullptr;
		VkDevice m_logicalDevice = nullptr;

	};

	template class CommandPoolVk<VulkanTarget::VK_1_0>;
	template class CommandPoolVk<VulkanTarget::VK_LATEST>;

}

#endif
