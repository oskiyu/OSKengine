#pragma once

#include "PhysicsEntity.h"
#include "Terrain.h"

#include "ProfilingUnit.h"

#include <vector>

namespace OSK {

	class PhysicsScene {

	public:

		void Simulate(const float_t& deltaTime, const float_t& step = 1.0f) {
			pUnit.Start();

			const float finalDelta = deltaTime * step;

			for (auto& i : Entities) {
				if (i->Type == PhysicalEntityType::STATIC)
					continue;

				i->ApplyForce(ConstForce);
				Vector3f accel = i->GetAcceleration();

				i->Velocity = i->Velocity + accel * finalDelta;
				i->EntityTransform->AddPosition(i->Velocity * finalDelta);
				i->Force = { 0.0f };

				i->AngularVelocity = i->AngularVelocity + i->AngularAcceleration * finalDelta;
				i->EntityTransform->AddRotation(i->AngularVelocity * finalDelta);

				i->Collision.BroadCollider.Box.Position = i->EntityTransform->GlobalPosition;
			}
		}

		void DetectCollisions(const deltaTime_t& deltaTime) {
			for (uint32_t a = 0; a < Entities.size(); a++) {
				for (uint32_t b = a + 1; b < Entities.size(); b++) {
					if (Entities[a]->Collision.IsColliding(Entities[b]->Collision)) {
					/*	const CollisionInfo info = Entities[a]->Collision.GetCollisionInfo(&Entities[b]->Collision);

						const float momentoA = Entities[a]->Velocity.GetLenght() * Entities[a]->Mass;
						const float momentoB = Entities[b]->Velocity.GetLenght() * Entities[a]->Mass;
						const float momentoTotal = momentoA + momentoB;
						const float forceA = momentoB * (1 / deltaTime);
						const float forceB = momentoA * (1 / deltaTime);

						Vector3f forceForA = info.A_to_B_Direction;
						Entities[a]->ApplyForce(forceForA * forceA);
						Entities[a]->AngularVelocity = Entities[a]->AngularVelocity + Entities[a]->GetTorque(info.A_to_B_Direction, Entities[a]->Velocity * forceA);

						Vector3f forceForB = info.B_to_A_Direction;
						Entities[b]->ApplyForce(forceForB * forceB);
						Entities[b]->AngularVelocity = Entities[b]->AngularVelocity + Entities[b]->GetTorque(info.B_to_A_Direction, Entities[b]->Velocity * forceB);*/
					}
				}
			}
		}

		void ResolveColisions() {
			for (auto& a : Entities) {
				const float terrainHeight = Terreno->GetHeight({ a->EntityTransform->GlobalPosition.X, a->EntityTransform->GlobalPosition.Z });

				if (a->EntityTransform->GlobalPosition.Y > terrainHeight) {
					a->EntityTransform->Position.Y = terrainHeight;
					//a->Velocity.Y = 0;
				}
			}

			pUnit.End();
		}

		Vector3f ConstForce = { 0, 9.8f, 0 };
		//Vector3f ConstForce = { 0, 0, 0 };

		std::vector<PhysicsEntity*> Entities;

		Terrain* Terreno = nullptr;

		ProfilingUnit pUnit{"OSK::PhysicsScene"};

	};

}
