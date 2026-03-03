#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "IComputePipeline.h"
#include "IPipelineVk.h"
#include "VulkanTarget.h"

namespace OSK::GRAPHICS {

	template <VulkanTarget Target>
	class OSKAPI_CALL ComputePipelineVk final : public IComputePipeline {

	public:

		/// @throws PipelineCreationException Si hay un error con Vulkan.
		void Create(const MaterialLayout& materialLayout, const PipelineCreateInfo& info);
		VkPipeline GetPipeline() const;

	private:

		IPipelineVk<Target> m_pipeline{};

	};

	template class ComputePipelineVk<VulkanTarget::VK_1_0>;
	template class ComputePipelineVk<VulkanTarget::VK_LATEST>;

}

#endif
