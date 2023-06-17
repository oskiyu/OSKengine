#pragma once

#include "OSKmacros.h"

struct VkDescriptorSetLayout_T;
using VkDescriptorSetLayout = VkDescriptorSetLayout_T*;

namespace OSK::GRAPHICS {

	struct MaterialLayoutSlot;

	/// <summary>
	/// Wrapper de un VkDescrpitorSetLayout.
	/// Se encarga de crear el layout a partir del MaterialLayoutSlot.
	/// </summary>
	class OSKAPI_CALL DescriptorLayoutVk final {

	public:

		/// @throws DescriptorLayoutCreationException Si hay algún problema nativo.
		DescriptorLayoutVk(const MaterialLayoutSlot* slotLayout);
		~DescriptorLayoutVk();

		VkDescriptorSetLayout GetLayout() const;
		const MaterialLayoutSlot* GetMaterialSlotLayout() const;

	private:

		VkDescriptorSetLayout layout = NULL;
		const MaterialLayoutSlot* slotLayout = nullptr;

	};

}
