#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "ApiCall.h"
#include "NumericTypes.h"

struct VkDescriptorSetLayout_T;
using VkDescriptorSetLayout = VkDescriptorSetLayout_T*;

namespace OSK::GRAPHICS {

	struct MaterialLayoutSlot;


	/// @brief Wrapper de un VkDescrpitorSetLayout.
	/// Se encarga de crear el layout a partir del MaterialLayoutSlot.
	class OSKAPI_CALL DescriptorLayoutVk final {

	public:

		/// @param slotLayout Layout del slot.
		/// @param maxSets Número máximo de sets.
		/// @throws DescriptorLayoutCreationException Si hay algún problema nativo.
		DescriptorLayoutVk(const MaterialLayoutSlot* slotLayout, USize32 maxSets);
		~DescriptorLayoutVk();

		VkDescriptorSetLayout GetLayout() const;
		const MaterialLayoutSlot* GetMaterialSlotLayout() const;

	private:

		VkDescriptorSetLayout m_layoutVk = nullptr;
		const MaterialLayoutSlot* m_slotLayout = nullptr;

	};

}

#endif
