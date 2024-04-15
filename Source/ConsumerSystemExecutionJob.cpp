#include "ConsumerSystemExecutionJob.h"

using namespace OSK;
using namespace OSK::ECS;


ConsumerSystemExecutionJob::ConsumerSystemExecutionJob(IConsumerSystem* system, EventQueueSpan events, TDeltaTime deltaTime)
	: m_system(system), m_eventRange(events), m_deltaTime(deltaTime) {

}

void ConsumerSystemExecutionJob::Execute() {
	m_system->Execute(m_deltaTime, m_eventRange);
}
