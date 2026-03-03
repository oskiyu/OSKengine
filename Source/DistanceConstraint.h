#pragma once

#include "ApiCall.h"
#include "NumericTypes.h"
#include "IConstraint.h"

namespace OSK::ECS {
	class PhysicsComponent;
	class TransformComponent3D;
}

namespace OSK::PHYSICS {

	class OSKAPI_CALL DistanceConstraint : public IConstraint {

	public:

		void Execute(
			ECS::PhysicsComponent* first,
			ECS::TransformComponent3D* firstTransform,
			ECS::PhysicsComponent* second,
			ECS::TransformComponent3D* secondTransform,
			TDeltaTime deltaTime) const override;

		float m_distance = 0.0f;

	};

}
