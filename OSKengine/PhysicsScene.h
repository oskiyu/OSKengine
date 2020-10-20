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

namespace OSK {

	//Motor de f�sicas.
	//Representa una escena en la que participan entidades f�sicass, que interaccionan entre s�.
	class OSKAPI_CALL PhysicsScene {

	public:
		
		//A�ade una entidad f�sica a la escena.
		//	<entity>: entidad a a�adir.
		inline void AddEntity(PhysicsEntity* entity) {
			Entities.push_back(entity);
		}

		//Quita una entidad f�sica de la escena.
		//Si la entidad no est� en la escena, no hace nada.
		//	<entity>: entidad a quitar.
		inline void RemoveEntity(PhysicsEntity* entity) {
			Entities.erase(std::remove(Entities.begin(), Entities.end(), entity), Entities.end());
		}

		//Simula y resuelve las colisiones de la escena.
		//	<deltaTime>: deltaTime.
		//	<step>: multiplicador delta.
		void Simulate(const deltaTime_t& deltaTime, const deltaTime_t& step);

		//Aceleraci�n que sufren todas las entidades de la escena.
		Vector3f GlobalAcceleration = { 0.0f, 9.8f, 0.0f };

		//Terreno de la escena.
		OSK::Terrain* FloorTerrain = nullptr;

		//Representa qu� simular� el motor de f�sicas.
		bitFlags_t SimulateFlags = PHYSICAL_SCENE_SIMULATE_VELOCITY | PHYSICAL_SCENE_SIMULATE_ACCEL | PHYSICAL_SCENE_SIMULATE_ROTATION;

		//Representa qu� har� el motor de f�sicas cuando se produzca una colisi�n.
		bitFlags_t ResolveFlags = PHYSICAL_SCENE_RESOLVE_ACCEL | PHYSICAL_SCENE_RESOLVE_ROTATION;

	private:

		std::vector<PhysicsEntity*> Entities = {};

		void simulateEntity(PhysicsEntity* entity, const deltaTime_t& delta);

		void resolveCollisions(PhysicsEntity* a, PhysicsEntity* b, const ColliderCollisionInfo& info, const deltaTime_t& delta);

	};

}
