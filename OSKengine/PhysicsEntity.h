#pragma once

#include "Transform.h"
#include "Collider.h"

namespace OSK {

	struct PhysicsEntity {

		PhysicalEntityType Type = PhysicalEntityType::MOVABLE;

		bool CanMoveX_p = true;
		bool CanMoveX_n = true;

		bool CanMoveY_p = true;
		bool CanMoveY_n = true;

		bool CanMoveZ_p = true;
		bool CanMoveZ_n = true;

		Transform* EntityTransform;

		Collider Collision;

		Vector3f Velocity = { 0.0f };
		Vector3f AngularVelocity = { 0.0f };

		inline static Vector3f GetTorque(const Vector3f& posOfForce, const Vector3f& force) {
			return posOfForce.Cross(force);
		}

		float Mass = 10.0f;

		void ApplyForce(const Vector3f& relativePosition, const Vector3f& force, const float& delta) {
			float diff = 1 - relativePosition.GetNormalized().Dot(force.GetNormalized());

			Velocity += force * diff * delta / Mass;

			AngularVelocity += GetTorque(relativePosition, force) * (1 - diff) * delta;
		}

	};

}
