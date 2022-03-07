#include "QueueFamilyIndices.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

bool QueueFamilyIndices::IsComplete() const {
	return graphicsFamily.has_value() && presentFamily.has_value();
}
