#include "ThreadedCommandPoolMap.h"

#include "OSKengine.h"
#include "IRenderer.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


ThreadedCommandPoolMap::ThreadedCommandPoolMap(const ICommandQueue* linkedCommandQueue) : m_linkedCommandQueue(linkedCommandQueue) {

}

ICommandPool* ThreadedCommandPoolMap::GetCommandPool(std::thread::id threadId) {
	// Si ya existe:
	{
		// Cerramos en modo read.
		std::shared_lock lock(m_readWriteMutex);

		auto pool = m_commandPoolPerThread.find(threadId);

		if (pool != m_commandPoolPerThread.end()) {
			return pool->second.GetPointer();
		}
	}

	// Si no, añadimos.
	//
	// Cerramos en modo escritura.
	std::unique_lock lock(m_readWriteMutex);

	OwnedPtr<ICommandPool> pool = Engine::GetRenderer()->CreateCommandPool(m_linkedCommandQueue);

	m_commandPoolPerThread[threadId] = pool.GetPointer();

	return pool.GetPointer();
}
