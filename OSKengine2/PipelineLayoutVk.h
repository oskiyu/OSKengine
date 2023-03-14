#pragma once

#include "IPipelineLayout.h"
#include "DynamicArray.hpp"
#include "OwnedPtr.h"

struct VkPipelineLayout_T;
typedef VkPipelineLayout_T* VkPipelineLayout;

namespace OSK::GRAPHICS {

	class DescriptorLayoutVk;

	class OSKAPI_CALL PipelineLayoutVk final : public IPipelineLayout {

	public:

		PipelineLayoutVk(const MaterialLayout* layout);
		~PipelineLayoutVk();

		VkPipelineLayout GetLayout() const;

	private:

		VkPipelineLayout layout = 0;

	};

}
