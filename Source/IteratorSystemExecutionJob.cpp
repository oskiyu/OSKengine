#include "IteratorSystemExecutionJob.h"

using namespace OSK;
using namespace OSK::ECS;


IteratorSystemExecutionJob::IteratorSystemExecutionJob(IIteratorSystem* system, std::span<GameObjectIndex> objects, TDeltaTime deltaTime)
	: m_system(system), m_objectRange(objects), m_deltaTime(deltaTime) {

}

void IteratorSystemExecutionJob::Execute() {
	m_system->Execute(m_deltaTime, m_objectRange);
}
