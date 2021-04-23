#include "PhysicsSystem.h"

#include "Math.h"
#include "ECS.h"
#include "CollisionComponent.h"
#include "GameObject.h"

using namespace OSK;
using namespace OSK::Collision;

Signature PhysicsSystem::GetSystemSignature() {
	Signature signature;

	signature.set(ECSsystem->GetComponentType<OSK::PhysicsComponent>());

	return signature;
}

void PhysicsSystem::OnTick(deltaTime_t deltaTime) {
	Simulate(deltaTime, 1.0f);
}

void PhysicsSystem::Simulate(deltaTime_t deltaTime, deltaTime_t step) {
	const deltaTime_t delta = deltaTime * step;

	for (auto& i : Objects) {
		PhysicsComponent* physics = &ECSsystem->GetComponent<PhysicsComponent>(i);
		Transform* transform = &ECSsystem->GameObjects[i]->Transform3D;

		simulateEntity(physics, transform, delta);
	}

	for (auto a : Objects) {
		for (auto b : Objects) {
			if (a == b)
				continue;

			PhysicsComponent* physicsA = &ECSsystem->GetComponent<PhysicsComponent>(a);
			PhysicsComponent* physicsB = &ECSsystem->GetComponent<PhysicsComponent>(b);

			ColliderCollisionInfo info = physicsA->Collision.GetCollisionInfo(physicsB->Collision);

			if (info.IsColliding)
				resolveCollisions(physicsA, physicsB, &ECSsystem->GameObjects[a]->Transform3D, &ECSsystem->GameObjects[b]->Transform3D, info, delta);
		}

		if (FloorTerrain) {
			PhysicsComponent* physics = &ECSsystem->GetComponent<PhysicsComponent>(a);
			Transform& transform = ECSsystem->GameObjects[a]->Transform3D;

			if (TerrainColissionType == PhysicalSceneTerrainResolveType::RESOLVE_DETAILED) {
				checkTerrainCollision(physics, &transform, delta);
			}
			else if (TerrainColissionType == PhysicalSceneTerrainResolveType::CHANGE_HEIGHT_ONLY) {
				const float terrainHeight = FloorTerrain->GetHeight({ transform.GlobalPosition.X, transform.GlobalPosition.Z }) + physics->Height;
				if (transform.GlobalPosition.Y > terrainHeight) {
					physics->Velocity.Y = 0;
					physics->CanMoveY_p = false;
					transform.LocalPosition.Y = terrainHeight;
				}
				else {
					physics->CanMoveY_p = true;
				}
			}
		}
	}
}

void PhysicsSystem::simulateEntity(PhysicsComponent* entity, Transform* transform, deltaTime_t delta) {
	if (entity->Type == PhysicalEntityMobilityType::STATIC)
		return;
	
	if (SimulateFlags & PHYSICAL_SCENE_SIMULATE_ACCEL)
		entity->Velocity += (GlobalAcceleration / entity->Mass) * delta;

	if (FloorTerrain) {
		
		if (TerrainColissionType == PhysicalSceneTerrainResolveType::RESOLVE_DETAILED) {
			const float height = FloorTerrain->GetHeight({ transform->GlobalPosition.X , transform->GlobalPosition.Z });
			if (transform->GlobalPosition.Y > height) {
				entity->Velocity.Y = 0;
				entity->CanMoveY_p = false;
				transform->LocalPosition.Y = height;

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
				entity->IsOnFloor = false;
			}
			else {
				entity->CanMoveY_p = true;
			}

			if (entity->IsOnFloor) {
				entity->CanMoveY_p = false;

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
				entity->IsOnFloor = false;
			}
			else {
				entity->CanMoveY_p = true;
			}
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
		transform->AddPosition(entity->Velocity * delta);

	if (SimulateFlags & PHYSICAL_SCENE_SIMULATE_ROTATION)
		transform->RotateWorldSpace(entity->AngularVelocity.GetLenght() * delta, entity->AngularVelocity);

	entity->Collision.SetPosition(transform->GlobalPosition);

	if (SimulateFlags & PHYSICAL_SCENE_SIMULATE_ROTATION)
		entity->AngularVelocity *= (1 - (delta * delta));

	for (auto& i : entity->Collision.SatColliders)
		i.TransformPoints();
}

void PhysicsSystem::resolveCollisions(PhysicsComponent* a, PhysicsComponent* b, Transform* transformA, Transform* transformB, const ColliderCollisionInfo& info, deltaTime_t delta) {
	const Collision::SAT_CollisionInfo satInfo = info.SAT_1->GetCollisionInfo(*info.SAT_2);

	Vector3f MTV = satInfo.MinimunTranslationVector;

	//Vectores A->B y B->A.
	const Vector3f A_To_B = transformB->GlobalPosition - transformA->GlobalPosition;
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
		transformA->AddPosition(MTV_ForA);
	if (b->ResponseFlags & PHYSICAL_ENTITY_RESPONSE_MOVE_MTV)
		transformB->AddPosition(MTV_ForB);

	if (ResolveFlags & PHYSICAL_SCENE_RESOLVE_ROTATION) {
		if (a->Type == PhysicalEntityMobilityType::MOVABLE && a->ResponseFlags & PHYSICAL_ENTITY_RESPONSE_ROTATE && b->InteractionFlags & PHYSICAL_ENTITY_INTERACTION_GIVE_ROTATION) {

			Vector3f rotA = 0.0f;

			//Rotación.
			const Vector3f pointA = satInfo.PointA;
			
			rotA += PhysicsComponent::GetTorque(pointA, B_To_A.GetNormalized()) * 3 * delta;

			transformA->RotateWorldSpace(1 / a->Mass, rotA);

			a->AngularVelocity += (rotA);
		}

		if (b->Type == PhysicalEntityMobilityType::MOVABLE && b->ResponseFlags & PHYSICAL_ENTITY_RESPONSE_ROTATE && a->InteractionFlags & PHYSICAL_ENTITY_INTERACTION_GIVE_ROTATION) {

			Vector3f rotB = 0.0f;

			//Rotación.
			const Vector3f pointB = satInfo.PointB;

			rotB += PhysicsComponent::GetTorque(pointB, A_To_B.GetNormalized()) * 3 * delta;

			transformB->RotateWorldSpace(1 / b->Mass, rotB);

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

void PhysicsSystem::checkTerrainCollision(PhysicsComponent* entity, Transform* transform, deltaTime_t delta) {
	if (entity->Collision.GetCurrentBroadColliderType() == BroadColliderType::BOX_AABB) {
		resolveTerrainCollisionAABB(entity, transform, delta, entity->Collision.BroadCollider.Box);
	}
	else {
		CollisionBox box;
		box.Position = entity->Collision.BroadCollider.Sphere.Position;
		box.Size = { entity->Collision.BroadCollider.Sphere.Radius };

		resolveTerrainCollisionAABB(entity, transform, delta, box);
	}
}

void PhysicsSystem::resolveTerrainCollisionAABB(PhysicsComponent* entity, Transform* transform, deltaTime_t delta, const CollisionBox& box) {
	Vector2f worldMin = { box.GetMin().X + box.Size.X, box.GetMin().Z + box.Size.Z };
	Vector2f worldMax = { box.GetMax().X + box.Size.X, box.GetMax().Z + box.Size.Z };

	if (worldMin.X < 0.0f)
		worldMin.X = 0.0f;
	if (worldMin.Y < 0.0f)
		worldMin.Y = 0.0f;

	if (worldMax.X > FloorTerrain->MapSizeInWorlds.X)
		worldMax.X = FloorTerrain->MapSizeInWorlds.X;
	if (worldMax.Y > FloorTerrain->MapSizeInWorlds.Y)
		worldMax.Y = FloorTerrain->MapSizeInWorlds.Y;

	Vector2i mapMin = { (int32_t)(worldMin.X / FloorTerrain->QuadSize.X),  (int32_t)(worldMin.Y / FloorTerrain->QuadSize.Y) };
	Vector2i mapMax = { (int32_t)(worldMax.X / FloorTerrain->QuadSize.X),  (int32_t)(worldMax.Y / FloorTerrain->QuadSize.Y) };

	Vector3f lowestPoint = { 0.0f };
	for (const auto& bb : entity->Collision.SatColliders)
		for (const auto& point : bb.GetPoints())
			if (point.Y > lowestPoint.Y)
				lowestPoint = point;

	bool canCollide = false;
	for (int32_t x = mapMin.X; x < mapMax.X; x++) {
		for (int32_t z = mapMin.Y; z < mapMax.Y; z++) {
			if (lowestPoint.Y > FloorTerrain->GetVertexHeight({ x, z })) {
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

	for (int32_t x = mapMin.X; x < mapMax.X - TerrainCollisionDetectionPrecision; x += TerrainCollisionDetectionPrecision) {
		for (int32_t z = mapMin.Y; z < mapMax.Y - TerrainCollisionDetectionPrecision; z += TerrainCollisionDetectionPrecision) {
			{
				SAT_Collider triangleA;

				std::vector<Vector3f> pointsOfFace;

				pointsOfFace.push_back({ x * FloorTerrain->QuadSize.X,
										FloorTerrain->GetVertexHeight({x, z}),
										z * FloorTerrain->QuadSize.Y });

				pointsOfFace.push_back({ (x + TerrainCollisionDetectionPrecision) * FloorTerrain->QuadSize.X,
										FloorTerrain->GetVertexHeight({(x + TerrainCollisionDetectionPrecision), z}),
										z * FloorTerrain->QuadSize.Y });

				pointsOfFace.push_back({ x * FloorTerrain->QuadSize.X,
										FloorTerrain->GetVertexHeight({x, (z + TerrainCollisionDetectionPrecision)}),
										(z + TerrainCollisionDetectionPrecision) * FloorTerrain->QuadSize.Y });

				triangleA.AddFace(pointsOfFace.data(), pointsOfFace.size());

				for (auto& obb : entity->Collision.SatColliders) {
					if (obb.Intersects(triangleA)) {
						entity->IsOnFloor = true;

						SAT_CollisionInfo info = obb.GetCollisionInfo(triangleA);

						Vector3f normal = triangleA.GetAxes()[0].GetNormalized();

						SAT_Projection proj1 = obb.ProjectToAxis(normal);
						SAT_Projection proj2 = triangleA.ProjectToAxis(normal);
						const float overlapping = std::abs(proj1.GetOverlap(proj2));

						if (normal.Y > 0.0f)
							normal *= -1;

						normal *= overlapping;

						if (overlapping < TerrainCollisionDelta) {
							entity->Velocity.Y = 0.0f;
							resolveTerrainCollisionRotation(entity, transform, info, normal, delta);

							break;
						}

						resolveTerrainCollision(entity, transform, info, normal, delta);

						hasCollidedThisFrame = true;

						if (TerrainCollisionDetectionSingleTimePerFrame)
							break;
					}
				}
			}

			{
				SAT_Collider triangleB;

				std::vector<Vector3f> pointsOfFace;
				pointsOfFace.push_back({ (x + TerrainCollisionDetectionPrecision) * FloorTerrain->QuadSize.X,
										FloorTerrain->GetVertexHeight({(x + TerrainCollisionDetectionPrecision), (z + TerrainCollisionDetectionPrecision)}),
										(z + TerrainCollisionDetectionPrecision) * FloorTerrain->QuadSize.Y });
				pointsOfFace.push_back({ (x + TerrainCollisionDetectionPrecision) * FloorTerrain->QuadSize.X,
										FloorTerrain->GetVertexHeight({(x + TerrainCollisionDetectionPrecision), z}),
										z * FloorTerrain->QuadSize.Y });
				pointsOfFace.push_back({ x * FloorTerrain->QuadSize.X, FloorTerrain->GetVertexHeight({x,
										(z + TerrainCollisionDetectionPrecision)}),
										(z + TerrainCollisionDetectionPrecision) * FloorTerrain->QuadSize.Y });

				triangleB.AddFace(pointsOfFace.data(), pointsOfFace.size());

				for (auto& obb : entity->Collision.SatColliders) {
					if (obb.Intersects(triangleB)) {
						entity->IsOnFloor = true;

						SAT_CollisionInfo info = obb.GetCollisionInfo(triangleB);

						Vector3f normal = triangleB.GetAxes()[0].GetNormalized();

						SAT_Projection proj1 = obb.ProjectToAxis(normal);
						SAT_Projection proj2 = triangleB.ProjectToAxis(normal);
						const float overlapping = std::abs(proj1.GetOverlap(proj2));

						if (normal.Y > 0.0f)
							normal *= -1;

						normal *= overlapping;

						if (overlapping < TerrainCollisionDelta) {
							entity->Velocity.Y = 0.0f;
							resolveTerrainCollisionRotation(entity, transform, info, normal, delta);

							break;
						}

						resolveTerrainCollision(entity, transform, info, normal, delta);

						hasCollidedThisFrame = true;

						if (TerrainCollisionDetectionSingleTimePerFrame)
							break;
					}
				}
			}

			if (TerrainCollisionDetectionSingleTimePerFrame)
				if (hasCollidedThisFrame)
					return;
		}
	}
}

void PhysicsSystem::resolveTerrainCollision(PhysicsComponent* entity, Transform* transform, const SAT_CollisionInfo& info, const Vector3f& triangleNormal, deltaTime_t delta) {
	if (entity->ResponseFlags & PHYSICAL_ENTITY_RESPONSE_MOVE_MTV) {
		transform->AddPosition(triangleNormal);
	}

	for (auto& i : entity->Collision.SatColliders)
		i.TransformPoints();

	resolveTerrainCollisionRotation(entity, transform, info, triangleNormal, delta);
}

void PhysicsSystem::resolveTerrainCollisionRotation(PhysicsComponent* entity, Transform* transform, const Collision::SAT_CollisionInfo& info, const Vector3f& triangleNormal, deltaTime_t delta) {
	if (ResolveFlags & PHYSICAL_SCENE_RESOLVE_ROTATION) {
		if (entity->ResponseFlags & PHYSICAL_ENTITY_RESPONSE_ROTATE) {
			//Rotación.
			const Vector3f pointA = info.PointA;

			Vector3f rotA = PhysicsComponent::GetTorque(pointA, { 0.0f, -9.8f, 0.0f }) * delta * entity->Velocity.GetLenght();

			transform->RotateWorldSpace(entity->Velocity.GetLenght(), rotA);

			entity->AngularVelocity += rotA;
		}
	}

	for (auto& i : entity->Collision.SatColliders)
		i.TransformPoints();
}