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

	//Motor de físicas.
	//Representa una escena en la que participan entidades físicass, que interaccionan entre sí.
	class OSKAPI_CALL PhysicsScene {

	public:
		
		//Añade una entidad física a la escena.
		//	<entity>: entidad a añadir.
		inline void AddEntity(PhysicsEntity* entity) {
			Entities.push_back(entity);
		}

		//Quita una entidad física de la escena.
		//Si la entidad no está en la escena, no hace nada.
		//	<entity>: entidad a quitar.
		inline void RemoveEntity(PhysicsEntity* entity) {
			Entities.erase(std::remove(Entities.begin(), Entities.end(), entity), Entities.end());
		}

		//Simula y resuelve las colisiones de la escena.
		//	<deltaTime>: deltaTime.
		//	<step>: multiplicador delta.
		void Simulate(const deltaTime_t& deltaTime, const deltaTime_t& step);

		//Aceleración que sufren todas las entidades de la escena.
		Vector3f GlobalAcceleration = { 0.0f, 9.8f, 0.0f };

		//Terreno de la escena.
		OSK::Terrain* FloorTerrain = nullptr;

		//Representa qué simulará el motor de físicas.
		bitFlags_t SimulateFlags = PHYSICAL_SCENE_SIMULATE_VELOCITY | PHYSICAL_SCENE_SIMULATE_ACCEL | PHYSICAL_SCENE_SIMULATE_ROTATION;

		//Representa qué hará el motor de físicas cuando se produzca una colisión.
		bitFlags_t ResolveFlags = PHYSICAL_SCENE_RESOLVE_ACCEL | PHYSICAL_SCENE_RESOLVE_ROTATION;

	private:

		std::vector<PhysicsEntity*> Entities = {};

		void simulateEntity(PhysicsEntity* entity, const deltaTime_t& delta);

		void resolveCollisions(PhysicsEntity* a, PhysicsEntity* b, const ColliderCollisionInfo& info, const deltaTime_t& delta);

	};

}
