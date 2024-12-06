#pragma once

#include "ISystemController.h"
#include "PhysicsSystem.h"

namespace OSK::Editor::Controllers {

	class OSKAPI_CALL PhysicsSystemController : public TSystemController<ECS::PhysicsSystem> {

	public:

		OSK_LINK_SYSTEM_EDITOR_CONTROLLER(ECS::PhysicsSystem);

		explicit PhysicsSystemController(ECS::ISystem* system, Views::ISystemView* view);

		void Poll() override;

	private:

		Vector3f m_previousGravity = Vector3f::Zero;

	};

}
