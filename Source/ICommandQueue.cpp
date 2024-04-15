#include "ICommandQueue.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

ICommandQueue::ICommandQueue(QueueFamily family, GpuQueueType queueType, UIndex32 indexInsideFamily) : m_family(family), m_mainQueueType(queueType), m_queueIndex(indexInsideFamily) {}

CommandsSupport ICommandQueue::GetSupportedCommands() const {
	return m_family.support;
}
const QueueFamily& ICommandQueue::GetFamily() const {
	return m_family;
}

GpuQueueType ICommandQueue::GetQueueType() const {
	return m_mainQueueType;
}

UIndex32 ICommandQueue::GetQueueIndexInsideFamily() const {
	return m_queueIndex;
}
