#pragma once

#include "IPipelineLayout.h"
#include "DynamicArray.hpp"
#include "OwnedPtr.h"

struct VkPipelineLayout_T;
typedef VkPipelineLayout_T* VkPipelineLayout;

namespace OSK {

	class DescriptorLayoutVulkan;
	class MaterialLayout;

	class OSKAPI_CALL PipelineLayoutVulkan : public IPipelineLayout {

	public:

		PipelineLayoutVulkan(const MaterialLayout* layout);
		~PipelineLayoutVulkan();

		VkPipelineLayout GetLayout() const;

	private:

		VkPipelineLayout layout = 0;

	};

}
