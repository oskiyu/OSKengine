#include "PhysicsSystem.h"

#include "Math.h"
#include "ECS.h"
#include "CollisionComponent.h"
#include "GameObject.h"

using namespace OSK;
using namespace OSK::Collision;

Signature PhysicsSystem::GetSystemSignature() {
	Signature signature;

	signature.set(entityComponentSystem->GetComponentType<OSK::PhysicsComponent>());

	return signature;
}

void PhysicsSystem::OnTick(deltaTime_t deltaTime) {
	Simulate(deltaTime, 1.0f);
}

void PhysicsSystem::Simulate(deltaTime_t deltaTime, deltaTime_t step) {
	const deltaTime_t delta = deltaTime * step;

	for (auto& i : objects) {
		PhysicsComponent* physics = &entityComponentSystem->GetComponent<PhysicsComponent>(i);
		Transform* transform = entityComponentSystem->GetGameObjectByID(i)->GetTransform();

		simulateEntity(physics, transform, delta);
	}

	for (auto a : objects) {
		for (auto b : objects) {
			if (a == b)
				continue;

			PhysicsComponent* physicsA = &entityComponentSystem->GetComponent<PhysicsComponent>(a);
			PhysicsComponent* physicsB = &entityComponentSystem->GetComponent<PhysicsComponent>(b);

			ColliderCollisionInfo info = physicsA->GetCollider().GetCollisionInfo(physicsB->GetCollider());

			if (info.isColliding)
				resolveCollisions(physicsA, physicsB, entityComponentSystem->GetGameObjectByID(a)->GetTransform(), entityComponentSystem->GetGameObjectByID(b)->GetTransform(), info, delta);
		}

		if (terrain) {
			PhysicsComponent* physics = &entityComponentSystem->GetComponent<PhysicsComponent>(a);
			Transform& transform = *entityComponentSystem->GetGameObjectByID(a)->GetTransform();

			if (terrainColissionType == PhysicalSceneTerrainResolveType::RESOLVE_DETAILED) {
				checkTerrainCollision(physics, &transform, delta);
			}
			else if (terrainColissionType == PhysicalSceneTerrainResolveType::CHANGE_HEIGHT_ONLY) {
				const float terrainHeight = terrain->GetHeight({ transform.GetPosition().X, transform.GetPosition().Z }) + physics->gravityCenterHeight;
				if (transform.GetPosition().Y > terrainHeight) {
					physics->velocity.Y = 0;
					physics->canMoveY_p = false;
					transform.localPosition.Y = terrainHeight;
				}
				else {
					physics->canMoveY_p = true;
				}
			}
		}
	}
}

void PhysicsSystem::simulateEntity(PhysicsComponent* entity, Transform* transform, deltaTime_t delta) {
	if (entity->mobilityType == PhysicalEntityMobilityType::STATIC)
		return;
	
	if (simulateFlags & PHYSICAL_SCENE_SIMULATE_ACCEL)
		entity->velocity += (globalAcceleration / entity->mass) * delta;

	if (terrain) {
		
		if (terrainColissionType == PhysicalSceneTerrainResolveType::RESOLVE_DETAILED) {
			const float height = terrain->GetHeight({ transform->GetPosition().X , transform->GetPosition().Z });
			if (transform->GetPosition().Y > height) {
				entity->velocity.Y = 0;
				entity->canMoveY_p = false;
				transform->localPosition.Y = height;

				const float multiply = entity->mass / 3 * terrain->frictionCoefficient;

				if (entity->velocity.X > 0)
					entity->velocity.X -= delta * multiply;
				if (entity->velocity.X < 0)
					entity->velocity.X += delta * multiply;

				if (entity->velocity.Z > 0)
					entity->velocity.Z -= delta * multiply;
				if (entity->velocity.Z < 0)
					entity->velocity.Z += delta * multiply;

				entity->angularVelocity *= (1 - delta);
				entity->isOnFloor = false;
			}
			else {
				entity->canMoveY_p = true;
			}

			if (entity->isOnFloor) {
				entity->canMoveY_p = false;

				const float multiply = entity->mass / 3 * terrain->frictionCoefficient;

				if (entity->velocity.X > 0)
					entity->velocity.X -= delta * multiply;
				if (entity->velocity.X < 0)
					entity->velocity.X += delta * multiply;

				if (entity->velocity.Z > 0)
					entity->velocity.Z -= delta * multiply;
				if (entity->velocity.Z < 0)
					entity->velocity.Z += delta * multiply;

				entity->angularVelocity *= (1 - delta);
				entity->isOnFloor = false;
			}
			else {
				entity->canMoveY_p = true;
			}
		}
	}
	
	if (!entity->canMoveX_p && entity->velocity.X > 0)
		entity->velocity.X = 0;
	if (!entity->canMoveX_n && entity->velocity.X < 0)
		entity->velocity.X = 0;

	if (!entity->canMoveY_p && entity->velocity.Y > 0)
		entity->velocity.Y = 0;
	if (!entity->canMoveY_n && entity->velocity.Y < 0)
		entity->velocity.Y = 0;

	if (!entity->canMoveZ_p && entity->velocity.Z > 0)
		entity->velocity.Z = 0;
	if (!entity->canMoveZ_n && entity->velocity.Z < 0)
		entity->velocity.Z = 0;

	if (simulateFlags & PHYSICAL_SCENE_SIMULATE_VELOCITY)
		transform->AddPosition(entity->velocity * delta);

	if (simulateFlags & PHYSICAL_SCENE_SIMULATE_ROTATION)
		transform->RotateWorldSpace(entity->angularVelocity.GetLenght() * delta, entity->angularVelocity);

	entity->GetCollider().SetPosition(transform->GetPosition());

	if (simulateFlags & PHYSICAL_SCENE_SIMULATE_ROTATION)
		entity->angularVelocity *= (1 - (delta * delta));

	//entity->GetCollider().
}

void PhysicsSystem::resolveCollisions(PhysicsComponent* a, PhysicsComponent* b, Transform* transformA, Transform* transformB, const ColliderCollisionInfo& info, deltaTime_t delta) {
	const Collision::SAT_CollisionInfo satInfo = info.sat1->GetCollisionInfo(*info.sat2);

	Vector3f MTV = satInfo.minimunTranslationVector;

	//Vectores A->B y B->A.
	const Vector3f A_To_B = transformB->GetPosition() - transformA->GetPosition();
	const Vector3f B_To_A = -A_To_B;

	//MTVs para A y B.
	Vector3f MTV_ForA = -MTV / 2;
	Vector3f MTV_ForB = MTV / 2;

	Vector3f aVelocity = A_To_B.GetNormalized() * b->velocity.GetLenght() * delta;
	Vector3f bVelocity = B_To_A.GetNormalized() * a->velocity.GetLenght() * delta;

#pragma region MTV correction.

	if (a->mobilityType == PhysicalEntityMobilityType::STATIC) {
		MTV_ForA = 0.0f;
		MTV_ForB = MTV;
	}
	else if(b->mobilityType == PhysicalEntityMobilityType::STATIC) {
		MTV_ForA = -MTV;
		MTV_ForB = 0.0f;
	}
	else {
		if (!a->canMoveX_p && MTV.X > 0) {
			MTV_ForA.X = 0;
			MTV_ForB.X *= 2;

			aVelocity.X = 0;
			bVelocity.X = 0;
		}
		if (!a->canMoveX_n && MTV.X < 0) {
			MTV_ForA.X = 0;
			MTV_ForB.X *= 2;

			aVelocity.X = 0;
			bVelocity.X = 0;
		}
		if (!b->canMoveX_p && MTV.X > 0) {
			MTV_ForA.X *= 2;
			MTV_ForB.X = 0;

			aVelocity.X = 0;
			bVelocity.X = 0;
		}
		if (!b->canMoveX_n && MTV.X < 0) {
			MTV_ForA.X *= 2;
			MTV_ForB.X = 0;

			aVelocity.X = 0;
			bVelocity.X = 0;
		}

		if (!a->canMoveY_p && MTV.Y > 0) {
			MTV_ForA.Y = 0;
			MTV_ForB.Y *= 2;

			aVelocity.Y = 0;
			bVelocity.Y = 0;
		}
		if (!a->canMoveY_n && MTV.Y < 0) {
			MTV_ForA.Y = 0;
			MTV_ForB.Y *= 2;

			aVelocity.Y = 0;
			bVelocity.Y = 0;
		}
		if (!b->canMoveY_p && MTV.X > 0) {
			MTV_ForA.Y *= 2;
			MTV_ForB.Y = 0;

			aVelocity.Y = 0;
			bVelocity.Y = 0;
		}
		if (!b->canMoveY_n && MTV.X < 0) {
			MTV_ForA.Y *= 2;
			MTV_ForB.Y = 0;

			aVelocity.Y = 0;
			bVelocity.Y = 0;
		}

		if (!a->canMoveZ_p && MTV.Z > 0) {
			MTV_ForA.Z = 0;
			MTV_ForB.Z *= 2;

			aVelocity.Z = 0;
			bVelocity.Z = 0;
		}
		if (!a->canMoveZ_n && MTV.Z < 0) {
			MTV_ForA.Z = 0;
			MTV_ForB.Z *= 2;

			aVelocity.Z = 0;
			bVelocity.Z = 0;
		}
		if (!b->canMoveZ_p && MTV.Z > 0) {
			MTV_ForA.Z *= 2;
			MTV_ForB.Z = 0;

			aVelocity.Z = 0;
			bVelocity.Z = 0;
		}
		if (!b->canMoveZ_n && MTV.Z < 0) {
			MTV_ForA.Z *= 2;
			MTV_ForB.Z = 0;

			aVelocity.Z = 0;
			bVelocity.Z = 0;
		}
	}

#pragma endregion

	if (a->responseFlags & PHYSICAL_ENTITY_RESPONSE_MOVE_MTV)
		transformA->AddPosition(MTV_ForA);
	if (b->responseFlags & PHYSICAL_ENTITY_RESPONSE_MOVE_MTV)
		transformB->AddPosition(MTV_ForB);

	if (resolveFlags & PHYSICAL_SCENE_RESOLVE_ROTATION) {
		if (a->mobilityType == PhysicalEntityMobilityType::MOVABLE && a->responseFlags & PHYSICAL_ENTITY_RESPONSE_ROTATE && b->interactionFlags & PHYSICAL_ENTITY_INTERACTION_GIVE_ROTATION) {

			Vector3f rotA = 0.0f;

			//Rotaci�n.
			const Vector3f pointA = satInfo.pointA;
			
			rotA += PhysicsComponent::GetTorque(pointA, B_To_A.GetNormalized()) * 3 * delta;

			transformA->RotateWorldSpace(1 / a->mass, rotA);

			a->angularVelocity += (rotA);
		}

		if (b->mobilityType == PhysicalEntityMobilityType::MOVABLE && b->responseFlags & PHYSICAL_ENTITY_RESPONSE_ROTATE && a->interactionFlags & PHYSICAL_ENTITY_INTERACTION_GIVE_ROTATION) {

			Vector3f rotB = 0.0f;

			//Rotaci�n.
			const Vector3f pointB = satInfo.pointB;

			rotB += PhysicsComponent::GetTorque(pointB, A_To_B.GetNormalized()) * 3 * delta;

			transformB->RotateWorldSpace(1 / b->mass, rotB);

			b->angularVelocity += (rotB);
		}

	}


	if (resolveFlags & PHYSICAL_SCENE_RESOLVE_ACCEL) {
		if (a->mobilityType == PhysicalEntityMobilityType::MOVABLE && a->responseFlags & PHYSICAL_ENTITY_RESPONSE_ACCEL && b->interactionFlags & PHYSICAL_ENTITY_INTERACTION_GIVE_ACCEL)
			a->velocity += MTV_ForA.GetNormalized() * a->mass * b->velocity.GetLenght() * delta / a->mass;

		if (b->mobilityType == PhysicalEntityMobilityType::MOVABLE && b->responseFlags & PHYSICAL_ENTITY_RESPONSE_ACCEL && a->interactionFlags & PHYSICAL_ENTITY_INTERACTION_GIVE_ACCEL)
			b->velocity += MTV_ForB.GetNormalized() * b->mass * a->velocity.GetLenght() * delta / b->mass;
	}
}

void PhysicsSystem::checkTerrainCollision(PhysicsComponent* entity, Transform* transform, deltaTime_t delta) {
	if (entity->GetCollider().GetCurrentBroadColliderType() == BroadColliderType::BOX_AABB) {
		resolveTerrainCollisionAABB(entity, transform, delta, entity->GetCollider().GetBroadCollisionBox());
	}
	else {
		CollisionBox box;
		box.position = entity->GetCollider().GetBroadCollisionSphere().position;
		box.size = { entity->GetCollider().GetBroadCollisionSphere().radius };

		resolveTerrainCollisionAABB(entity, transform, delta, box);
	}
}

void PhysicsSystem::resolveTerrainCollisionAABB(PhysicsComponent* entity, Transform* transform, deltaTime_t delta, const CollisionBox& box) {
	Vector2f worldMin = { box.GetMin().X + box.size.X, box.GetMin().Z + box.size.Z };
	Vector2f worldMax = { box.GetMax().X + box.size.X, box.GetMax().Z + box.size.Z };

	if (worldMin.X < 0.0f)
		worldMin.X = 0.0f;
	if (worldMin.Y < 0.0f)
		worldMin.Y = 0.0f;

	if (worldMax.X > terrain->mapSizeInWorlds.X)
		worldMax.X = terrain->mapSizeInWorlds.X;
	if (worldMax.Y > terrain->mapSizeInWorlds.Y)
		worldMax.Y = terrain->mapSizeInWorlds.Y;

	Vector2i mapMin = { (int32_t)(worldMin.X / terrain->quadSize.X),  (int32_t)(worldMin.Y / terrain->quadSize.Y) };
	Vector2i mapMax = { (int32_t)(worldMax.X / terrain->quadSize.X),  (int32_t)(worldMax.Y / terrain->quadSize.Y) };

	Vector3f lowestPoint = { 0.0f };
	for (const auto& bb : entity->GetCollider().satColliders)
		for (const auto& point : bb.GetPoints())
			if (point.Y > lowestPoint.Y)
				lowestPoint = point;

	bool canCollide = false;
	for (int32_t x = mapMin.X; x < mapMax.X; x++) {
		for (int32_t z = mapMin.Y; z < mapMax.Y; z++) {
			if (lowestPoint.Y > terrain->GetVertexHeight({ x, z })) {
				canCollide = true;

				break;
			}

			if (canCollide)
				break;
		}
	}

	if (!canCollide)
		return;

	bool hasCollidedThisFrame = false;

	for (int32_t x = mapMin.X; x < mapMax.X - terrainCollisionDetectionPrecision; x += terrainCollisionDetectionPrecision) {
		for (int32_t z = mapMin.Y; z < mapMax.Y - terrainCollisionDetectionPrecision; z += terrainCollisionDetectionPrecision) {
			{
				SAT_Collider triangleA;

				std::vector<Vector3f> pointsOfFace;

				pointsOfFace.push_back({ x * terrain->quadSize.X,
										terrain->GetVertexHeight({x, z}),
										z * terrain->quadSize.Y });

				pointsOfFace.push_back({ (x + terrainCollisionDetectionPrecision) * terrain->quadSize.X,
										terrain->GetVertexHeight({(x + terrainCollisionDetectionPrecision), z}),
										z * terrain->quadSize.Y });

				pointsOfFace.push_back({ x * terrain->quadSize.X,
										terrain->GetVertexHeight({x, (z + terrainCollisionDetectionPrecision)}),
										(z + terrainCollisionDetectionPrecision) * terrain->quadSize.Y });

				triangleA.AddFace(pointsOfFace.data(), pointsOfFace.size());

				for (auto& obb : entity->GetCollider().satColliders) {
					if (obb.Intersects(triangleA)) {
						entity->isOnFloor = true;

						SAT_CollisionInfo info = obb.GetCollisionInfo(triangleA);

						Vector3f normal = triangleA.GetAxes()[0].GetNormalized();

						SAT_Projection proj1 = obb.ProjectToAxis(normal);
						SAT_Projection proj2 = triangleA.ProjectToAxis(normal);
						const float overlapping = std::abs(proj1.GetOverlap(proj2));

						if (normal.Y > 0.0f)
							normal *= -1;

						normal *= overlapping;

						if (overlapping < terrainCollisionDelta) {
							entity->velocity.Y = 0.0f;
							resolveTerrainCollisionRotation(entity, transform, info, normal, delta);

							break;
						}

						resolveTerrainCollision(entity, transform, info, normal, delta);

						hasCollidedThisFrame = true;

						if (terrainCollisionDetectionSingleTimePerFrame)
							break;
					}
				}
			}

			{
				SAT_Collider triangleB;

				std::vector<Vector3f> pointsOfFace;
				pointsOfFace.push_back({ (x + terrainCollisionDetectionPrecision) * terrain->quadSize.X,
										terrain->GetVertexHeight({(x + terrainCollisionDetectionPrecision), (z + terrainCollisionDetectionPrecision)}),
										(z + terrainCollisionDetectionPrecision) * terrain->quadSize.Y });
				pointsOfFace.push_back({ (x + terrainCollisionDetectionPrecision) * terrain->quadSize.X,
										terrain->GetVertexHeight({(x + terrainCollisionDetectionPrecision), z}),
										z * terrain->quadSize.Y });
				pointsOfFace.push_back({ x * terrain->quadSize.X, terrain->GetVertexHeight({x,
										(z + terrainCollisionDetectionPrecision)}),
										(z + terrainCollisionDetectionPrecision) * terrain->quadSize.Y });

				triangleB.AddFace(pointsOfFace.data(), pointsOfFace.size());

				for (auto& obb : entity->GetCollider().satColliders) {
					if (obb.Intersects(triangleB)) {
						entity->isOnFloor = true;

						SAT_CollisionInfo info = obb.GetCollisionInfo(triangleB);

						Vector3f normal = triangleB.GetAxes()[0].GetNormalized();

						SAT_Projection proj1 = obb.ProjectToAxis(normal);
						SAT_Projection proj2 = triangleB.ProjectToAxis(normal);
						const float overlapping = std::abs(proj1.GetOverlap(proj2));

						if (normal.Y > 0.0f)
							normal *= -1;

						normal *= overlapping;

						if (overlapping < terrainCollisionDelta) {
							entity->velocity.Y = 0.0f;
							resolveTerrainCollisionRotation(entity, transform, info, normal, delta);

							break;
						}

						resolveTerrainCollision(entity, transform, info, normal, delta);

						hasCollidedThisFrame = true;

						if (terrainCollisionDetectionSingleTimePerFrame)
							break;
					}
				}
			}

			if (terrainCollisionDetectionSingleTimePerFrame)
				if (hasCollidedThisFrame)
					return;
		}
	}
}

void PhysicsSystem::resolveTerrainCollision(PhysicsComponent* entity, Transform* transform, const SAT_CollisionInfo& info, const Vector3f& triangleNormal, deltaTime_t delta) {
	if (entity->responseFlags & PHYSICAL_ENTITY_RESPONSE_MOVE_MTV) {
		transform->AddPosition(triangleNormal);
	}

	for (auto& i : entity->GetCollider().satColliders)
		i.TransformPoints();

	resolveTerrainCollisionRotation(entity, transform, info, triangleNormal, delta);
}

void PhysicsSystem::resolveTerrainCollisionRotation(PhysicsComponent* entity, Transform* transform, const Collision::SAT_CollisionInfo& info, const Vector3f& triangleNormal, deltaTime_t delta) {
	if (resolveFlags & PHYSICAL_SCENE_RESOLVE_ROTATION) {
		if (entity->responseFlags & PHYSICAL_ENTITY_RESPONSE_ROTATE) {
			//Rotaci�n.
			const Vector3f pointA = info.pointA;

			Vector3f rotA = PhysicsComponent::GetTorque(pointA, { 0.0f, -9.8f, 0.0f }) * delta * entity->velocity.GetLenght();

			transform->RotateWorldSpace(entity->velocity.GetLenght(), rotA);

			entity->angularVelocity += rotA;
		}
	}

	for (auto& i : entity->GetCollider().satColliders)
		i.TransformPoints();
}