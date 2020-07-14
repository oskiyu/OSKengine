#pragma once

#include "VulkanRenderizableObject.h"

namespace OSK::VULKAN {

	struct OSKAPI_CALL DrawCommand {

		VulkanRenderizableObject* object;

		bool hasToBindTexture = false;

	};

}