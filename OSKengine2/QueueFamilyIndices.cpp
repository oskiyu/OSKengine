#include "QueueFamilyIndices.h"

using namespace OSK;

bool QueueFamilyIndices::IsComplete() const {
	return graphicsFamily.has_value() && presentFamily.has_value();
}
