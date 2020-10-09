#pragma once

#include "Transform.h"
#include "Collider.h"

namespace OSK {

	struct PhysicsEntity {

		PhysicalEntityType Type = PhysicalEntityType::MOVABLE;

		Transform* EntityTransform;

		Collider Collision;

		Vector3f Velocity;
		Vector3f Force;
		Vector3f AngularAcceleration;
		Vector3f AngularVelocity;
		
		float_t Mass;

		inline Vector3f GetAcceleration() const {
			return Force / Mass;
		}

		inline Vector3f GetTorque(const Vector3f& posOfForce, const Vector3f& force) const {
			return posOfForce.Cross(force);
		}

		inline void ApplyTorque(const Vector3f& torque) {
			AngularAcceleration = AngularAcceleration + torque;
		}

		inline void ApplyForce(const Vector3f& force) {
			Force = Force + force;
		}

		inline void AddLinearVelocity(const Vector3f& delta) {
			Velocity = Velocity + delta;
		}

		inline void SetLinearVelocity(const Vector3f& velocity) {
			Velocity = velocity;
		}

	};

}
