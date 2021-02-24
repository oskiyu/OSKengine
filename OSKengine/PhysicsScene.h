#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "PhysicsEntity.h"
#include "Terrain.h"

#include "ProfilingUnit.h"
#include "RayCast.h"

#include "SAT_CollisionInfo.h"
#include "PhysicalEntityType.h"

#include <vector>

#include "ECS.h"

namespace OSK {

	//Motor de f�sicas.
	//Representa una escena en la que participan entidades f�sicass, que interaccionan entre s�.
	class OSKAPI_CALL PhysicsScene : public ECS::System {

	public:

		void OnTick(deltaTime_t deltaTime) override;
		Signature GetSystemSignature() override;
		
		//Simula y resuelve las colisiones de la escena.
		//	<deltaTime>: deltaTime.
		//	<step>: multiplicador delta.
		void Simulate(deltaTime_t deltaTime, deltaTime_t step);

		//Aceleraci�n que sufren todas las entidades de la escena.
		Vector3f GlobalAcceleration = { 0.0f, 9.8f, 0.0f };

		//Terreno de la escena.
		OSK::Terrain* FloorTerrain = nullptr;

		//Representa qu� simular� el motor de f�sicas.
		bitFlags_t SimulateFlags = PHYSICAL_SCENE_SIMULATE_VELOCITY | PHYSICAL_SCENE_SIMULATE_ACCEL | PHYSICAL_SCENE_SIMULATE_ROTATION;

		//Representa qu� har� el motor de f�sicas cuando se produzca una colisi�n.
		bitFlags_t ResolveFlags = PHYSICAL_SCENE_RESOLVE_ACCEL | PHYSICAL_SCENE_RESOLVE_ROTATION;

		//Indica qu� har� el motor de f�sicas al detectar una colisi�n con el terreno.
		PhysicalSceneTerrainResolveType TerrainColissionType = PhysicalSceneTerrainResolveType::RESOLVE_DETAILED;

		//La precisi�n con la que se calcula la colisi�n SAT - Tri�ngulo.
		//Cuanto mayor sea, menos tri�ngulos se comprobar�n.
		//SIEMPRE DEBE SER MAYOR QUE 1.
		//S�lo para PhysicalSceneTerrainResolveType::PHYSICAL_SCENE_RESOLVE_DETAILED;
		int32_t TerrainCollisionDetectionPrecision = 1;

		//Distancia m�nima que debe de haber entre la entidad y el suelo para que se produzca respuesta.
		//S�lo para PhysicalSceneTerrainResolveType::PHYSICAL_SCENE_RESOLVE_DETAILED;
		float TerrainCollisionDelta = 0.2f;

		//Si es true, s�lo se calcular� una colisi�n por frame
		//S�lo para PhysicalSceneTerrainResolveType::PHYSICAL_SCENE_RESOLVE_DETAILED;
		bool TerrainCollisionDetectionSingleTimePerFrame = false;

	private:

		void simulateEntity(PhysicsComponent* entity, Transform* transform, deltaTime_t delta);

		void resolveCollisions(PhysicsComponent* a, PhysicsComponent* b, Transform* transformA, Transform* transformB, const ColliderCollisionInfo& info, deltaTime_t delta);

		void checkTerrainCollision(PhysicsComponent* entity, Transform* transform, deltaTime_t delta);

		void resolveTerrainCollision(PhysicsComponent* entity, Transform* transform, const Collision::SAT_CollisionInfo& info, const Vector3f& triangleNormal, deltaTime_t delta);

		void resolveTerrainCollisionAABB(PhysicsComponent* entity, Transform* transform, deltaTime_t delta, const CollisionBox& box);

		void resolveTerrainCollisionRotation(PhysicsComponent* entity, Transform* transform, const Collision::SAT_CollisionInfo& info, const Vector3f& triangleNormal, deltaTime_t delta);

	};

}
