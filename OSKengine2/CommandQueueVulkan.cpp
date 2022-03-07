#include "CommandQueueVulkan.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void CommandQueueVulkan::SetQueue(VkQueue queue) {
	this->queue = queue;
}

VkQueue CommandQueueVulkan::GetQueue() const {
	return queue;
}
