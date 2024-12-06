#include "PhysicsSystemController.h"

#include "PhysicsSystemView.h"

OSK::Editor::Controllers::PhysicsSystemController::PhysicsSystemController(ECS::ISystem* system, Views::ISystemView* view) 
: TSystemController<ECS::PhysicsSystem>(system, view) 
{
	view->As<Views::PhysicsSystemView>()->SetGravity(GetSystem()->GetGravity());
}

void OSK::Editor::Controllers::PhysicsSystemController::Poll() {
	const auto currentGravity = GetSystem()->GetGravity();

	if (currentGravity != m_previousGravity) {
		GetView()->As<Views::PhysicsSystemView>()->SetGravity(GetSystem()->GetGravity());
		m_previousGravity = currentGravity;
	}
}
