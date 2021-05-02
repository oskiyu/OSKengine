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

	/// <summary>
	/// Sistema que funciona como motor de físicas.
	/// Representa una escena en la que participan entidades físicass, que interaccionan entre sí.
	/// </summary>
	class OSKAPI_CALL PhysicsSystem : public ECS::System {

	public:

		/// <summary>
		/// Función OnTick.
		/// Procesa las físicas.
		/// </summary>
		/// <param name="deltaTime">Delta.</param>
		void OnTick(deltaTime_t deltaTime) override;


		Signature GetSystemSignature() override;
		
		/// <summary>
		/// Simula y resuelve las colisiones de la escena.
		/// </summary>
		/// <param name="deltaTime">Delta.</param>
		/// <param name="step">Multiplicador de delta.</param>
		void Simulate(deltaTime_t deltaTime, deltaTime_t step);

		/// <summary>
		/// Aceleración que sufren todas las entidades de la escena.
		/// 'Gravedad'.
		/// </summary>
		Vector3f GlobalAcceleration = { 0.0f, 9.8f, 0.0f };

		/// <summary>
		/// Terreno de la escena.
		/// </summary>
		OSK::Terrain* FloorTerrain = nullptr;

		/// <summary>
		/// Representa qué simulará el motor de físicas.
		/// </summary>
		bitFlags_t SimulateFlags = PHYSICAL_SCENE_SIMULATE_VELOCITY | PHYSICAL_SCENE_SIMULATE_ACCEL | PHYSICAL_SCENE_SIMULATE_ROTATION;

		/// <summary>
		/// Representa qué hará el motor de físicas cuando se produzca una colisión.
		/// </summary>
		bitFlags_t ResolveFlags = PHYSICAL_SCENE_RESOLVE_ACCEL | PHYSICAL_SCENE_RESOLVE_ROTATION;

		/// <summary>
		/// Indica qué hará el motor de físicas al detectar una colisión con el terreno.
		/// </summary>
		PhysicalSceneTerrainResolveType TerrainColissionType = PhysicalSceneTerrainResolveType::RESOLVE_DETAILED;

		/// <summary>
		/// La precisión con la que se calcula la colisión SAT - Triángulo.
		/// Cuanto mayor sea, menos triángulos se comprobarán.
		/// SIEMPRE DEBE SER MAYOR QUE 1.
		/// Sólo para PhysicalSceneTerrainResolveType::PHYSICAL_SCENE_RESOLVE_DETAILED;
		/// </summary>
		int32_t TerrainCollisionDetectionPrecision = 1;

		/// <summary>
		/// Distancia mínima que debe de haber entre la entidad y el suelo para que se produzca respuesta.
		/// Sólo para PhysicalSceneTerrainResolveType::PHYSICAL_SCENE_RESOLVE_DETAILED;
		/// </summary>
		float TerrainCollisionDelta = 0.2f;

		/// <summary>
		/// Si es true, sólo se calculará una colisión por frame
		/// Sólo para PhysicalSceneTerrainResolveType::PHYSICAL_SCENE_RESOLVE_DETAILED;
		/// </summary>
		bool TerrainCollisionDetectionSingleTimePerFrame = false;

	private:

		/// <summary>
		/// Simula una entidad:
		/// cambia su posición, velocidad y aceleración.
		/// </summary>
		/// <param name="entity">Entidad simulada.</param>
		/// <param name="transform">Transform de la entidad.</param>
		/// <param name="delta">Delta.</param>
		void simulateEntity(PhysicsComponent* entity, Transform* transform, deltaTime_t delta);

		/// <summary>
		/// Resuelve una colisión entre dos entidades.
		/// </summary>
		/// <param name="a">Entidad 1.</param>
		/// <param name="b">Entidad 2.</param>
		/// <param name="transformA">Transform de la primera entidad.</param>
		/// <param name="transformB">Transform de la segunda entidad.</param>
		/// <param name="info">Información de la colisión.</param>
		/// <param name="delta">Delta.</param>
		void resolveCollisions(PhysicsComponent* a, PhysicsComponent* b, Transform* transformA, Transform* transformB, const ColliderCollisionInfo& info, deltaTime_t delta);

		/// <summary>
		/// Comprueba la colisión de una entidad con el terreno.
		/// </summary>
		/// <param name="entity">Entidad.</param>
		/// <param name="transform">Transform de la entidad.</param>
		/// <param name="delta">Delta.</param>
		void checkTerrainCollision(PhysicsComponent* entity, Transform* transform, deltaTime_t delta);

		/// <summary>
		/// Resuelve la colisión de una entidad con el terreno.
		/// </summary>
		/// <param name="entity">Enitdad.</param>
		/// <param name="transform">Transform de la entidad.</param>
		/// <param name="info">Información de la colisión.</param>
		/// <param name="triangleNormal">Normal del triángulo del terreno con el que se ha colisionado.</param>
		/// <param name="delta">Delta.</param>
		void resolveTerrainCollision(PhysicsComponent* entity, Transform* transform, const Collision::SAT_CollisionInfo& info, const Vector3f& triangleNormal, deltaTime_t delta);

		/// <summary>
		/// Comprueba la colisión de una entidad con el terreno.
		/// </summary>
		/// <param name="entity">Entidad.</param>
		/// <param name="transform">Transform de la entidad.</param>
		/// <param name="delta">Delta.</param>
		/// <param name="box">AABB del terreno.</param>
		void resolveTerrainCollisionAABB(PhysicsComponent* entity, Transform* transform, deltaTime_t delta, const CollisionBox& box);

		/// <summary>
		/// Resuelve la colisión de una entidad con el terreno, afectando a su rotación.
		/// </summary>
		/// <param name="entity">Entidad.</param>
		/// <param name="transform">Transform de la entidad.</param>
		/// <param name="info">Información de la colisión.</param>
		/// <param name="triangleNormal">Normal del triángulo del terreno con el que se ha colisionado.</param>
		/// <param name="delta">Delta.</param>
		void resolveTerrainCollisionRotation(PhysicsComponent* entity, Transform* transform, const Collision::SAT_CollisionInfo& info, const Vector3f& triangleNormal, deltaTime_t delta);

	};

}
