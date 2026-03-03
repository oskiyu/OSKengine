#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "ApiCall.h"
#include "NumericTypes.h"
#include "VulkanTarget.h"

struct VkDescriptorSetLayout_T;
using VkDescriptorSetLayout = VkDescriptorSetLayout_T*;

namespace OSK::GRAPHICS {

	struct MaterialLayoutSlot;


	/// @brief Wrapper de un VkDescrpitorSetLayout.
	/// Se encarga de crear el layout a partir del MaterialLayoutSlot.
	template <VulkanTarget Target>
	class OSKAPI_CALL DescriptorLayoutVk final {

	public:

		/// @param slotLayout Layout del slot.
		/// @param maxSets Número máximo de sets.
		/// @throws DescriptorLayoutCreationException Si hay algún problema nativo.
		DescriptorLayoutVk(const MaterialLayoutSlot* slotLayout);
		~DescriptorLayoutVk();

		VkDescriptorSetLayout GetLayout() const;
		const MaterialLayoutSlot* GetMaterialSlotLayout() const;

		USize32 GetNumDescriptors() const;

	private:

		VkDescriptorSetLayout m_layoutVk = nullptr;
		const MaterialLayoutSlot* m_slotLayout = nullptr;

		USize32 m_numDescriptors = 0;

	};

	template class DescriptorLayoutVk<VulkanTarget::VK_1_0>;
	template class DescriptorLayoutVk<VulkanTarget::VK_LATEST>;

}

#endif
