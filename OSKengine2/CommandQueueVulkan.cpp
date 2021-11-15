#include "CommandQueueVulkan.h"

using namespace OSK;

void CommandQueueVulkan::SetQueue(VkQueue queue) {
	this->queue = queue;
}

VkQueue CommandQueueVulkan::GetQueue() const {
	return queue;
}
