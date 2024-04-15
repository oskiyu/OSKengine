#pragma once

#include "IJob.h"
#include "IConsumerSystem.h"
#include "GameObject.h"

#include <span>


namespace OSK::ECS {

	class OSKAPI_CALL ConsumerSystemExecutionJob : public IJob {

	public:

		OSK_JOB("OSK::ConsumerSystemExecutionJob");

		ConsumerSystemExecutionJob(
			IConsumerSystem* system,
			EventQueueSpan events,
			TDeltaTime deltaTime);

		void Execute() override;

	private:

		IConsumerSystem* m_system = nullptr;
		EventQueueSpan m_eventRange;
		TDeltaTime m_deltaTime = 0.0f;

	};

}
