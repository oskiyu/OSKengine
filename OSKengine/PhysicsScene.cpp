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
	entity->Velocity += (GlobalAcceleration / entity->Mass) * delta;

	if (FloorTerrain) {
		const float height = FloorTerrain->GetHeight({ entity->EntityTransform->GlobalPosition.X , entity->EntityTransform->GlobalPosition.Z });
		if (entity->EntityTransform->GlobalPosition.Y > height) {
			entity->Velocity.Y = 0;
			entity->CanMoveY_p = false;
			entity->EntityTransform->Position.Y = height;

			const float multiply = entity->Mass / 3 * FloorTerrain->FrictionCoefficient;

			if (entity->Velocity.X > 0)
				entity->Velocity.X -= delta * multiply;
			if (entity->Velocity.X < 0)
				entity->Velocity.X += delta * multiply;

			if (entity->Velocity.Z > 0)
				entity->Velocity.Z -= delta * multiply;
			if (entity->Velocity.Z < 0)
				entity->Velocity.Z += delta * multiply;


			/*	if (entity->AngularVelocity.X > 0)
					entity->AngularVelocity.X -= delta * multiply * 90;
				if (entity->AngularVelocity.X < 0)
					entity->AngularVelocity.X += delta * multiply * 90;

				if (entity->AngularVelocity.Y > 0)
					entity->AngularVelocity.Y -= delta * multiply * 90;
				if (entity->AngularVelocity.Y < 0)
					entity->AngularVelocity.Y += delta * multiply * 90;

				if (entity->AngularVelocity.Z > 0)
					entity->AngularVelocity.Z -= delta * multiply * 90;
				if (entity->AngularVelocity.Z < 0)
					entity->AngularVelocity.Z += delta * multiply * 90;*/

					//	entity->AngularVelocity -= entity->AngularVelocity * delta * multiply;
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

	entity->EntityTransform->AddPosition(entity->Velocity * delta);
	entity->EntityTransform->AddRotation(entity->AngularVelocity * delta);
	entity->Collision.SetPosition(entity->EntityTransform->GlobalPosition);

	entity->AngularVelocity *= (1 - (delta * delta));
}

void PhysicsScene::resolveCollisions(PhysicsEntity* a, PhysicsEntity* b, const ColliderCollisionInfo& info, const deltaTime_t& delta) {
	const Collision::SAT_CollisionInfo satInfo = info.SAT_1->GetCollisionInfo(*info.SAT_2);

	const Vector3f MTV = satInfo.MinimunTranslationVector;

	const Vector3f A_To_B = b->EntityTransform->GlobalPosition - a->EntityTransform->GlobalPosition;
	const Vector3f B_To_A = -A_To_B;

	Vector3f MTV_ForA = -MTV / 2;
	Vector3f MTV_ForB = MTV / 2;

	Vector3f aVelocity = A_To_B.GetNormalized() * b->Velocity.GetLenght() * delta;
	Vector3f bVelocity = A_To_B.GetNormalized() * a->Velocity.GetLenght() * delta;

#pragma region MTV correction.

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

#pragma endregion

	a->EntityTransform->AddPosition(MTV_ForA);
	b->EntityTransform->AddPosition(MTV_ForB);

	Vector3f pointA = A_To_B / info.SAT_2->BoxTransform.GlobalScale;
	Vector3f pointB = B_To_A / info.SAT_1->BoxTransform.GlobalScale;

	Vector3f rotA = 0.0f;
	Vector3f rotB = 0.0f;

	rotA = PhysicsEntity::GetTorque(pointA, MTV_ForA.GetNormalized()) * 360 * delta;
	rotB = PhysicsEntity::GetTorque(pointB, MTV_ForB.GetNormalized()) * 360 * delta;

	//
	const Vector3f projectionTo = MTV_ForA.GetNormalized();
	std::vector<Vector3f> points = {};
	float currentProjection = std::numeric_limits<float>::min();
	for (const auto& i : info.SAT_2->GetPoints()) {
		const Vector3f p = i - info.SAT_2->BoxTransform.GlobalPosition;
		float proj = p.Dot(projectionTo);

		if (CompareFloats(proj, currentProjection)) {
			points.push_back(p);
		}
		else if (proj > currentProjection) {
			points.clear();
			points.push_back(p);
			currentProjection = proj;
		}
	}

	if (points.size() == 1) {
		pointA = points[0];
		pointB = -points[0];
	}
	else {
		Vector3f average = { 0.0f };
		for (const auto& i : points) {
			average += i / points.size();
		}

		pointB = average;

		pointA = info.SAT_1->BoxTransform.GlobalPosition - (average + info.SAT_2->BoxTransform.GlobalPosition);
	}

	//rotA += PhysicsEntity::GetTorque(pointA, -MTV_ForA.GetNormalized()) * 360 * delta * d;
	//rotB += PhysicsEntity::GetTorque(pointB, MTV_ForB.GetNormalized()) * 360 * delta * d;

	a->EntityTransform->AddRotation(rotA / a->Mass);
	b->EntityTransform->AddRotation(rotB / b->Mass);

	a->AngularVelocity += (rotA / a->Mass);
	b->AngularVelocity += (rotB / b->Mass);

	a->Velocity += B_To_A.GetNormalized() * a->Mass * b->Velocity.GetLenght() * delta / a->Mass;
	b->Velocity += A_To_B.GetNormalized() * b->Mass * a->Velocity.GetLenght() * delta / b->Mass;

	//a->ApplyForce(pointA, MTV_ForA.GetNormalized(), delta);
	//b->ApplyForce(pointB, MTV_ForB.GetNormalized(), delta);
}
