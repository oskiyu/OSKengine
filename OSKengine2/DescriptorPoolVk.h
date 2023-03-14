#pragma once

#include "OSKmacros.h"

struct VkDescriptorPool_T;
using VkDescriptorPool = VkDescriptorPool_T*;

namespace OSK::GRAPHICS {

	class DescriptorLayoutVk;

	/// <summary>
	/// Wrapper de un VkDescriptorPool.
	/// Se encarga de crear el pool a partir del MaterialLayoutSlot.
	/// </summary>
	class OSKAPI_CALL DescriptorPoolVk {

	public:

		/// <summary>
		/// Crea el descriptor pool, usado para crear y destruir descriptor sets.
		/// </summary>
		/// <param name="layout">Layout del slot.</param>
		/// <param name="maxSets">Número máximo de sets creables con este pool.</param>
		DescriptorPoolVk(const DescriptorLayoutVk& layout, TSize maxSets);
		~DescriptorPoolVk();

		VkDescriptorPool GetPool() const;

	private:

		VkDescriptorPool pool = NULL;

	};

}
