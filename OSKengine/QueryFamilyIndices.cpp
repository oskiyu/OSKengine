#include "QueueFamilyIndices.h"

namespace OSK::VULKAN {

	bool QueueFamilyIndices::IsComplete() {
		return GraphicsFamily.has_value() && PresentFamily.has_value();
	}

}