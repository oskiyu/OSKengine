#include "QueueFamilyIndices.h"

namespace OSK::VULKAN {

	bool QueueFamilyIndices::IsComplete() const {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}

}