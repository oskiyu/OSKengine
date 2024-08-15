#include "PhysicsComponentController3D.h"

#include "PhysicsComponentView.h"

#include "UiTextView.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::Editor;

Controllers::PhysicsComponentController3D::PhysicsComponentController3D(GameObjectIndex object, void* componentData, Views::IComponentView* view)
	: Controllers::TComponentController<ECS::PhysicsComponent>(object, componentData, view) {

}

void Controllers::PhysicsComponentController3D::Poll() {
	const PhysicsComponent& physics = *GetComponent();
	Views::PhysicsComponentView* view = GetView()->As<Views::PhysicsComponentView>();

	if (!m_hasPreviousState) {
		physics.IsImmovable()
			? view->SetImmobile()
			: view->SetMass(physics.GetMass());

		view->SetVelocity(physics.GetVelocity());
		view->SetAcceleration(physics.GetAcceleration());

		m_wasImmovile = physics.IsImmovable();
		m_previousMass = physics.GetMass();
		m_previousVelocity = physics.GetVelocity();
		m_previousAccel = physics.GetAcceleration();

		m_hasPreviousState = true;
	}
	else {
		if (m_wasImmovile != physics.IsImmovable() || m_previousMass != physics.GetMass()) {
			physics.IsImmovable()
				? view->SetImmobile()
				: view->SetMass(physics.GetMass());

			m_wasImmovile = physics.IsImmovable();
			m_previousMass = physics.GetMass();
		}

		if (m_previousVelocity != physics.GetVelocity()) {
			view->SetVelocity(physics.GetVelocity());
			m_previousVelocity = physics.GetVelocity();
		}

		if (m_previousAccel!= physics.GetAcceleration()) {
			view->SetAcceleration(physics.GetAcceleration());
			m_previousAccel = physics.GetAcceleration();
		}
	}
}
