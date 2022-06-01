#pragma once

#include "OSKmacros.h"

struct VkDescriptorPool_T;
using VkDescriptorPool = VkDescriptorPool_T*;

namespace OSK::GRAPHICS {

	class DescriptorLayoutVulkan;

	/// <summary>
	/// Wrapper de un VkDescriptorPool.
	/// Se encarga de crear el pool a partir del MaterialLayoutSlot.
	/// </summary>
	class OSKAPI_CALL DescriptorPoolVulkan {

	public:

		/// <summary>
		/// Crea el descriptor pool, usado para crear y destruir descriptor sets.
		/// </summary>
		/// <param name="layout">Layout del slot.</param>
		/// <param name="maxSets">Número máximo de sets creables con este pool.</param>
		DescriptorPoolVulkan(const DescriptorLayoutVulkan& layout, TSize maxSets);
		~DescriptorPoolVulkan();

		VkDescriptorPool GetPool() const;

	private:

		VkDescriptorPool pool = NULL;

	};

}
