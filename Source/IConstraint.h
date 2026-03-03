#pragma once

#include "ApiCall.h"

// Para TDeltaTime.
#include "NumericTypes.h"

#include "GameObject.h"

namespace OSK::ECS {
	class PhysicsComponent;
	class TransformComponent3D;
}

namespace OSK::PHYSICS {

	class OSKAPI_CALL IConstraint {

	public:

		virtual ~IConstraint() = default;

		virtual void Execute(
			ECS::PhysicsComponent* first,
			ECS::TransformComponent3D* firstTransform,
			ECS::PhysicsComponent* second,
			ECS::TransformComponent3D* secondTransform,
			TDeltaTime deltaTime) const = 0;

		ECS::GameObjectIndex m_first  = ECS::GameObjectIndex::CreateEmpty();
		ECS::GameObjectIndex m_second = ECS::GameObjectIndex::CreateEmpty();

	};

}
