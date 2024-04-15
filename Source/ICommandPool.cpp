#include "ICommandPool.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

ICommandPool::ICommandPool(CommandsSupport supportedCommands, GpuQueueType mainType) : m_supportedCommands(supportedCommands), m_mainQueueType(mainType) {

}

CommandsSupport ICommandPool::GetSupportedCommands() const {
	return m_supportedCommands;
}

GpuQueueType ICommandPool::GetLinkedQueueType() const {
	return m_mainQueueType;
}
