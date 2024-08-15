#pragma once

#include "IComponentController.h"
#include "PhysicsComponent.h"

namespace OSK::Editor::Controllers {

	class OSKAPI_CALL PhysicsComponentController3D : public TComponentController<ECS::PhysicsComponent> {

	public:

		OSK_LINK_COMPONENT_EDITOR_CONTROLLER(ECS::PhysicsComponent);

		PhysicsComponentController3D(
			ECS::GameObjectIndex object,
			void* componentData,
			Views::IComponentView* view);

	private:

		void Poll() override;

		bool m_hasPreviousState = false;

		bool m_wasImmovile = false;
		float m_previousMass = 0.0f;
		Vector3f m_previousVelocity = Vector3f::Zero;
		Vector3f m_previousAccel = Vector3f::Zero;

	};

}
