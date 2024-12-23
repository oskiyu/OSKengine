#include "ICommandQueue.h"

#include "GpuQueueTypes.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

ICommandQueue::ICommandQueue(GpuQueueType queueType) : m_queueType(queueType) {}

CommandsSupport ICommandQueue::GetSupportedCommands() const {
	switch (m_queueType) {
	case GpuQueueType::MAIN:
		return CommandsSupport::GRAPHICS | CommandsSupport::COMPUTE | CommandsSupport::PRESENTATION;
	case GpuQueueType::PRESENTATION:
		return CommandsSupport::PRESENTATION;
	case GpuQueueType::ASYNC_TRANSFER:
		return CommandsSupport::TRANSFER;
	}
}

GpuQueueType ICommandQueue::GetQueueType() const {
	return m_queueType;
}
