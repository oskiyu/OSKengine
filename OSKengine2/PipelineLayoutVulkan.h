#pragma once

#include "IPipelineLayout.h"
#include "DynamicArray.hpp"

struct VkPipelineLayout_T;
typedef VkPipelineLayout_T* VkPipelineLayout;

struct VkDescriptorSetLayout_T;
typedef VkDescriptorSetLayout_T* VkDescriptorSetLayout;

namespace OSK {

	class MaterialLayout;

	class OSKAPI_CALL PipelineLayoutVulkan : public IPipelineLayout {

	public:

		PipelineLayoutVulkan(const MaterialLayout* layout);
		~PipelineLayoutVulkan();

		VkPipelineLayout GetLayout() const;

	private:

		DynamicArray<VkDescriptorSetLayout> descLayouts;
		VkPipelineLayout layout = 0;

	};

}
