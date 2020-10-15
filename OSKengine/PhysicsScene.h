#pragma once

#include "PhysicsEntity.h"
#include "Terrain.h"

#include "ProfilingUnit.h"
#include "RayCast.h"

#include <vector>
#include "SAT_CollisionInfo.h"

namespace OSK {

	class PhysicsScene {

	public:

		OSK::Terrain* FloorTerrain = nullptr;
		
		inline void AddEntity(PhysicsEntity* entity) {
			Entities.push_back(entity);
		}

		void Simulate(const deltaTime_t& deltaTime, const deltaTime_t& step);

		Vector3f GlobalAcceleration = { 0.0f, 9.8f, 0.0f };

	private:

		std::vector<PhysicsEntity*> Entities = {};

		void simulateEntity(PhysicsEntity* entity, const deltaTime_t& delta);

		void resolveCollisions(PhysicsEntity* a, PhysicsEntity* b, const ColliderCollisionInfo& info, const deltaTime_t& delta);

	};

}
