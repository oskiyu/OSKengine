#include "PhysicsScene.h"

#include "Math.h"

using namespace OSK;

void PhysicsScene::Simulate(const deltaTime_t& deltaTime, const deltaTime_t& step) {
	const deltaTime_t delta = deltaTime * step;

	for (auto& i : Entities)
		simulateEntity(i, delta);

	for (uint32_t a = 0; a < Entities.size(); a++) {
		for (uint32_t b = a + 1; b < Entities.size(); b++) {
			ColliderCollisionInfo info = Entities[a]->Collision.GetCollisionInfo(Entities[b]->Collision);

			if (info.IsColliding)
				resolveCollisions(Entities[a], Entities[b], info, delta);
		}
	}
}

void PhysicsScene::simulateEntity(PhysicsEntity* entity, const deltaTime_t& delta) {
	if (entity->Type == PhysicalEntityMobilityType::STATIC)
		return;
	
	if (SimulateFlags & PHYSICAL_SCENE_SIMULATE_ACCEL)
		entity->Velocity += (GlobalAcceleration / entity->Mass) * delta;

	if (FloorTerrain) {
		const float height = FloorTerrain->GetHeight({ entity->EntityTransform->GlobalPosition.X , entity->EntityTransform->GlobalPosition.Z });
		if (entity->EntityTransform->GlobalPosition.Y > height) {
			entity->Velocity.Y = 0;
			entity->CanMoveY_p = false;
			entity->EntityTransform->LocalPosition.Y = height;

			const float multiply = entity->Mass / 3 * FloorTerrain->FrictionCoefficient;

			if (entity->Velocity.X > 0)
				entity->Velocity.X -= delta * multiply;
			if (entity->Velocity.X < 0)
				entity->Velocity.X += delta * multiply;

			if (entity->Velocity.Z > 0)
				entity->Velocity.Z -= delta * multiply;
			if (entity->Velocity.Z < 0)
				entity->Velocity.Z += delta * multiply;

			entity->AngularVelocity *= (1 - delta);
		}
		else {
			entity->CanMoveY_p = true;
		}
	}

	if (!entity->CanMoveX_p && entity->Velocity.X > 0)
		entity->Velocity.X = 0;
	if (!entity->CanMoveX_n && entity->Velocity.X < 0)
		entity->Velocity.X = 0;

	if (!entity->CanMoveY_p && entity->Velocity.Y > 0)
		entity->Velocity.Y = 0;
	if (!entity->CanMoveY_n && entity->Velocity.Y < 0)
		entity->Velocity.Y = 0;

	if (!entity->CanMoveZ_p && entity->Velocity.Z > 0)
		entity->Velocity.Z = 0;
	if (!entity->CanMoveZ_n && entity->Velocity.Z < 0)
		entity->Velocity.Z = 0;

	if (SimulateFlags & PHYSICAL_SCENE_SIMULATE_VELOCITY)
		entity->EntityTransform->AddPosition(entity->Velocity * delta);

	if (SimulateFlags & PHYSICAL_SCENE_SIMULATE_ROTATION)
		entity->EntityTransform->RotateWorldSpace(entity->AngularVelocity.GetLenght() * delta, entity->AngularVelocity);

	entity->Collision.SetPosition(entity->EntityTransform->GlobalPosition);

	if (SimulateFlags & PHYSICAL_SCENE_SIMULATE_ROTATION)
		entity->AngularVelocity *= (1 - (delta * delta));
}

void PhysicsScene::resolveCollisions(PhysicsEntity* a, PhysicsEntity* b, const ColliderCollisionInfo& info, const deltaTime_t& delta) {
	const Collision::SAT_CollisionInfo satInfo = info.SAT_1->GetCollisionInfo(*info.SAT_2);

	Vector3f MTV = satInfo.MinimunTranslationVector;

	//Vectores A->B y B->A.
	const Vector3f A_To_B = b->EntityTransform->GlobalPosition - a->EntityTransform->GlobalPosition;
	const Vector3f B_To_A = -A_To_B;

	//MTVs para A y B.
	Vector3f MTV_ForA = -MTV / 2;
	Vector3f MTV_ForB = MTV / 2;

	Vector3f aVelocity = A_To_B.GetNormalized() * b->Velocity.GetLenght() * delta;
	Vector3f bVelocity = B_To_A.GetNormalized() * a->Velocity.GetLenght() * delta;

#pragma region MTV correction.

	if (a->Type == PhysicalEntityMobilityType::STATIC) {
		MTV_ForA = 0.0f;
		MTV_ForB = MTV;
	}
	else if(b->Type == PhysicalEntityMobilityType::STATIC) {
		MTV_ForA = -MTV;
		MTV_ForB = 0.0f;
	}
	else {
		if (!a->CanMoveX_p && MTV.X > 0) {
			MTV_ForA.X = 0;
			MTV_ForB.X *= 2;

			aVelocity.X = 0;
			bVelocity.X = 0;
		}
		if (!a->CanMoveX_n && MTV.X < 0) {
			MTV_ForA.X = 0;
			MTV_ForB.X *= 2;

			aVelocity.X = 0;
			bVelocity.X = 0;
		}
		if (!b->CanMoveX_p && MTV.X > 0) {
			MTV_ForA.X *= 2;
			MTV_ForB.X = 0;

			aVelocity.X = 0;
			bVelocity.X = 0;
		}
		if (!b->CanMoveX_n && MTV.X < 0) {
			MTV_ForA.X *= 2;
			MTV_ForB.X = 0;

			aVelocity.X = 0;
			bVelocity.X = 0;
		}

		if (!a->CanMoveY_p && MTV.Y > 0) {
			MTV_ForA.Y = 0;
			MTV_ForB.Y *= 2;

			aVelocity.Y = 0;
			bVelocity.Y = 0;
		}
		if (!a->CanMoveY_n && MTV.Y < 0) {
			MTV_ForA.Y = 0;
			MTV_ForB.Y *= 2;

			aVelocity.Y = 0;
			bVelocity.Y = 0;
		}
		if (!b->CanMoveY_p && MTV.X > 0) {
			MTV_ForA.Y *= 2;
			MTV_ForB.Y = 0;

			aVelocity.Y = 0;
			bVelocity.Y = 0;
		}
		if (!b->CanMoveY_n && MTV.X < 0) {
			MTV_ForA.Y *= 2;
			MTV_ForB.Y = 0;

			aVelocity.Y = 0;
			bVelocity.Y = 0;
		}

		if (!a->CanMoveZ_p && MTV.Z > 0) {
			MTV_ForA.Z = 0;
			MTV_ForB.Z *= 2;

			aVelocity.Z = 0;
			bVelocity.Z = 0;
		}
		if (!a->CanMoveZ_n && MTV.Z < 0) {
			MTV_ForA.Z = 0;
			MTV_ForB.Z *= 2;

			aVelocity.Z = 0;
			bVelocity.Z = 0;
		}
		if (!b->CanMoveZ_p && MTV.Z > 0) {
			MTV_ForA.Z *= 2;
			MTV_ForB.Z = 0;

			aVelocity.Z = 0;
			bVelocity.Z = 0;
		}
		if (!b->CanMoveZ_n && MTV.Z < 0) {
			MTV_ForA.Z *= 2;
			MTV_ForB.Z = 0;

			aVelocity.Z = 0;
			bVelocity.Z = 0;
		}
	}

#pragma endregion

	if (a->ResponseFlags & PHYSICAL_ENTITY_RESPONSE_MOVE_MTV)
		a->EntityTransform->AddPosition(MTV_ForA);
	if (b->ResponseFlags & PHYSICAL_ENTITY_RESPONSE_MOVE_MTV)
		b->EntityTransform->AddPosition(MTV_ForB);

	if (ResolveFlags & PHYSICAL_SCENE_RESOLVE_ROTATION) {
		if (a->Type == PhysicalEntityMobilityType::MOVABLE && a->ResponseFlags & PHYSICAL_ENTITY_RESPONSE_ROTATE && b->InteractionFlags & PHYSICAL_ENTITY_INTERACTION_GIVE_ROTATION) {

			Vector3f rotA = 0.0f;

			//Rotación.
			const Vector3f pointA = satInfo.PointA;
			
			rotA += PhysicsEntity::GetTorque(pointA, B_To_A.GetNormalized()) * 3 * delta;

			a->EntityTransform->RotateWorldSpace(1 / a->Mass, rotA);

			a->AngularVelocity += (rotA);
		}

		if (b->Type == PhysicalEntityMobilityType::MOVABLE && b->ResponseFlags & PHYSICAL_ENTITY_RESPONSE_ROTATE && a->InteractionFlags & PHYSICAL_ENTITY_INTERACTION_GIVE_ROTATION) {

			Vector3f rotB = 0.0f;

			//Rotación.
			const Vector3f pointB = satInfo.PointB;

			rotB += PhysicsEntity::GetTorque(pointB, A_To_B.GetNormalized()) * 3 * delta;

			b->EntityTransform->RotateWorldSpace(1 / b->Mass, rotB);

			b->AngularVelocity += (rotB);
		}

	}


	if (ResolveFlags & PHYSICAL_SCENE_RESOLVE_ACCEL) {
		if (a->Type == PhysicalEntityMobilityType::MOVABLE && a->ResponseFlags & PHYSICAL_ENTITY_RESPONSE_ACCEL && b->InteractionFlags & PHYSICAL_ENTITY_INTERACTION_GIVE_ACCEL)
			a->Velocity += MTV_ForA.GetNormalized() * a->Mass * b->Velocity.GetLenght() * delta / a->Mass;

		if (b->Type == PhysicalEntityMobilityType::MOVABLE && b->ResponseFlags & PHYSICAL_ENTITY_RESPONSE_ACCEL && a->InteractionFlags & PHYSICAL_ENTITY_INTERACTION_GIVE_ACCEL)
			b->Velocity += MTV_ForB.GetNormalized() * b->Mass * a->Velocity.GetLenght() * delta / b->Mass;
	}
}
