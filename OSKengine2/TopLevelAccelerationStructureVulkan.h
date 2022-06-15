#pragma once

#include "ITopLevelAccelerationStructure.h"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class OSKAPI_CALL TopLevelAccelerationStructureVulkan : public ITopLevelAccelerationStructure {

	public:

		void Setup() override;

		VkAccelerationStructureKHR GetAccelerationStructure() const;

	private:

		VkAccelerationStructureKHR blasHandle = VK_NULL_HANDLE;
		VkAccelerationStructureKHR tlasHandle = VK_NULL_HANDLE;

	};

}
