#include "QueueFamilyIndices.h"

namespace OSK::VULKAN {

	bool QueueFamilyIndices::IsComplete() const {
		return GraphicsFamily.has_value() & PresentFamily.has_value();
	}

}