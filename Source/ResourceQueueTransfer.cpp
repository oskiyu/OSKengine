#include "ResourceQueueTransfer.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

ResourceQueueTransferInfo ResourceQueueTransferInfo::Empty() {
	ResourceQueueTransferInfo output{};
	output.transfer = false;

	return output;
}

ResourceQueueTransferInfo ResourceQueueTransferInfo::FromTo(const ICommandQueue* source, const ICommandQueue* destination) {
	ResourceQueueTransferInfo output{};
	output.transfer = true;
	output.sourceQueue = source;
	output.destinationQueue= destination;

	return output;
}
