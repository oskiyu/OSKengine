#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "IPipelineLayout.h"
#include "DynamicArray.hpp"
#include "VulkanTarget.h"

struct VkPipelineLayout_T;
typedef VkPipelineLayout_T* VkPipelineLayout;

namespace OSK::GRAPHICS {

	template <VulkanTarget> class DescriptorLayoutVk;

	template <VulkanTarget Target>
	class OSKAPI_CALL PipelineLayoutVk final : public IPipelineLayout {

	public:

		/// @throws PipelineLayoutCreationException Si hay algún error con el API nativo.
		PipelineLayoutVk(const MaterialLayout* layout);
		~PipelineLayoutVk();

		VkPipelineLayout GetLayout() const;

	private:

		VkPipelineLayout layout = 0;

	};

	template class PipelineLayoutVk<VulkanTarget::VK_1_0>;
	template class PipelineLayoutVk<VulkanTarget::VK_LATEST>;

}

#endif
