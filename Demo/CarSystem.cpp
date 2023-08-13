#include "CarSystem.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/EntityComponentSystem.h>

#include <OSKengine/Transform3D.h>
#include <OSKengine/PhysicsComponent.h>

#include "CarInputEvent.h"
#include "CarComponent.h"

void CarSystem::OnCreate() {
	
}

void CarSystem::OnTick(TDeltaTime) {
	OSK::ECS::EntityComponentSystem* ecs = OSK::Engine::GetEcs();

	for (const CarInputEvent& event : ecs->GetEventQueue<CarInputEvent>()) {
		const OSK::ECS::GameObjectIndex id = event.carObject;

		if (!(ecs->ObjectHasComponent<CarComponent>(id) && ecs->ObjectHasComponent<OSK::ECS::PhysicsComponent>(id)))
			continue;

		OSK::ECS::PhysicsComponent& physicsComponent = ecs->GetComponent<OSK::ECS::PhysicsComponent>(id);
		OSK::ECS::Transform3D& transform = ecs->GetComponent<OSK::ECS::Transform3D>(id);

		switch (event.type) {
		case CarInputEvent::Type::ACCELERATE:
			physicsComponent.ApplyForce( transform.GetForwardVector() * 5.0f);
			break;

		case CarInputEvent::Type::DECELERATE:
			physicsComponent.ApplyForce(-transform.GetForwardVector() * 5.0f);
			break;

		case CarInputEvent::Type::TURN_LEFT:
			break;

		case CarInputEvent::Type::TURN_RIGHT:
			break;
		}
	}
}
