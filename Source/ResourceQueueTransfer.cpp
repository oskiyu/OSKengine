#include "ResourceQueueTransfer.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

ResourceQueueTransferInfo ResourceQueueTransferInfo::Empty() {
	ResourceQueueTransferInfo output{};
	output.transfer = false;

	return output;
}

ResourceQueueTransferInfo ResourceQueueTransferInfo::FromTo(const QueueFamily& source, const QueueFamily& destination) {
	ResourceQueueTransferInfo output{};
	output.transfer = true;
	output.sourceFamily = source;
	output.destinationFamily = destination;

	return output;
}
