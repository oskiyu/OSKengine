#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "ApiCall.h"
#include "NumericTypes.h"

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
		/// <param name="maxSets">N�mero m�ximo de sets creables con este pool.</param>
		/// 
		/// @throws DescriptorPoolCreationException Si hay alg�n problema nativo.
		DescriptorPoolVk(const DescriptorLayoutVk& layout, USize32 maxSets);
		~DescriptorPoolVk();

		VkDescriptorPool GetPool() const;

	private:

		VkDescriptorPool pool = nullptr;

	};

}

#endif
