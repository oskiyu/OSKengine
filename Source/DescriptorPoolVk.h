#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "ApiCall.h"
#include "NumericTypes.h"
#include "VulkanTarget.h"

struct VkDescriptorPool_T;
using VkDescriptorPool = VkDescriptorPool_T*;

namespace OSK::GRAPHICS {

	template <VulkanTarget> class DescriptorLayoutVk;

	/// <summary>
	/// Wrapper de un VkDescriptorPool.
	/// Se encarga de crear el pool a partir del MaterialLayoutSlot.
	/// </summary>
	template <VulkanTarget Target>
	class OSKAPI_CALL DescriptorPoolVk {

	public:

		/// <summary>
		/// Crea el descriptor pool, usado para crear y destruir descriptor sets.
		/// </summary>
		/// <param name="layout">Layout del slot.</param>
		/// <param name="maxSets">Número máximo de sets creables con este pool.</param>
		/// 
		/// @throws DescriptorPoolCreationException Si hay algún problema nativo.
		DescriptorPoolVk(const DescriptorLayoutVk<Target>& layout, USize32 maxSets);
		~DescriptorPoolVk();

		VkDescriptorPool GetPool() const;

	private:

		VkDescriptorPool pool = nullptr;

	};

	template class DescriptorPoolVk<VulkanTarget::VK_1_0>;
	template class DescriptorPoolVk<VulkanTarget::VK_LATEST>;

}

#endif
