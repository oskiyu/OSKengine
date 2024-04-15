#pragma once

#include "IJob.h"
#include "IIteratorSystem.h"
#include "GameObject.h"

#include <span>


namespace OSK::ECS {

	class OSKAPI_CALL IteratorSystemExecutionJob : public IJob {

	public:

		OSK_JOB("OSK::IteratorSystemExecutionJob");

		IteratorSystemExecutionJob(
			IIteratorSystem* system, 
			std::span<GameObjectIndex> objects,
			TDeltaTime deltaTime);

		void Execute() override;

	private:

		IIteratorSystem* m_system = nullptr;
		std::span<GameObjectIndex> m_objectRange;
		TDeltaTime m_deltaTime = 0.0f;

	};

}
