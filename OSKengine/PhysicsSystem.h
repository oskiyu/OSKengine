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
	/// Sistema que funciona como motor de f�sicas.
	/// Representa una escena en la que participan entidades f�sicass, que interaccionan entre s�.
	/// </summary>
	class OSKAPI_CALL PhysicsSystem : public ECS::System {

	public:

		/// <summary>
		/// Funci�n OnTick.
		/// Procesa las f�sicas.
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
		/// Aceleraci�n que sufren todas las entidades de la escena.
		/// 'Gravedad'.
		/// </summary>
		Vector3f GlobalAcceleration = { 0.0f, 9.8f, 0.0f };

		/// <summary>
		/// Terreno de la escena.
		/// </summary>
		OSK::Terrain* FloorTerrain = nullptr;

		/// <summary>
		/// Representa qu� simular� el motor de f�sicas.
		/// </summary>
		bitFlags_t SimulateFlags = PHYSICAL_SCENE_SIMULATE_VELOCITY | PHYSICAL_SCENE_SIMULATE_ACCEL | PHYSICAL_SCENE_SIMULATE_ROTATION;

		/// <summary>
		/// Representa qu� har� el motor de f�sicas cuando se produzca una colisi�n.
		/// </summary>
		bitFlags_t ResolveFlags = PHYSICAL_SCENE_RESOLVE_ACCEL | PHYSICAL_SCENE_RESOLVE_ROTATION;

		/// <summary>
		/// Indica qu� har� el motor de f�sicas al detectar una colisi�n con el terreno.
		/// </summary>
		PhysicalSceneTerrainResolveType TerrainColissionType = PhysicalSceneTerrainResolveType::RESOLVE_DETAILED;

		/// <summary>
		/// La precisi�n con la que se calcula la colisi�n SAT - Tri�ngulo.
		/// Cuanto mayor sea, menos tri�ngulos se comprobar�n.
		/// SIEMPRE DEBE SER MAYOR QUE 1.
		/// S�lo para PhysicalSceneTerrainResolveType::PHYSICAL_SCENE_RESOLVE_DETAILED;
		/// </summary>
		int32_t TerrainCollisionDetectionPrecision = 1;

		/// <summary>
		/// Distancia m�nima que debe de haber entre la entidad y el suelo para que se produzca respuesta.
		/// S�lo para PhysicalSceneTerrainResolveType::PHYSICAL_SCENE_RESOLVE_DETAILED;
		/// </summary>
		float TerrainCollisionDelta = 0.2f;

		/// <summary>
		/// Si es true, s�lo se calcular� una colisi�n por frame
		/// S�lo para PhysicalSceneTerrainResolveType::PHYSICAL_SCENE_RESOLVE_DETAILED;
		/// </summary>
		bool TerrainCollisionDetectionSingleTimePerFrame = false;

	private:

		/// <summary>
		/// Simula una entidad:
		/// cambia su posici�n, velocidad y aceleraci�n.
		/// </summary>
		/// <param name="entity">Entidad simulada.</param>
		/// <param name="transform">Transform de la entidad.</param>
		/// <param name="delta">Delta.</param>
		void simulateEntity(PhysicsComponent* entity, Transform* transform, deltaTime_t delta);

		/// <summary>
		/// Resuelve una colisi�n entre dos entidades.
		/// </summary>
		/// <param name="a">Entidad 1.</param>
		/// <param name="b">Entidad 2.</param>
		/// <param name="transformA">Transform de la primera entidad.</param>
		/// <param name="transformB">Transform de la segunda entidad.</param>
		/// <param name="info">Informaci�n de la colisi�n.</param>
		/// <param name="delta">Delta.</param>
		void resolveCollisions(PhysicsComponent* a, PhysicsComponent* b, Transform* transformA, Transform* transformB, const ColliderCollisionInfo& info, deltaTime_t delta);

		/// <summary>
		/// Comprueba la colisi�n de una entidad con el terreno.
		/// </summary>
		/// <param name="entity">Entidad.</param>
		/// <param name="transform">Transform de la entidad.</param>
		/// <param name="delta">Delta.</param>
		void checkTerrainCollision(PhysicsComponent* entity, Transform* transform, deltaTime_t delta);

		/// <summary>
		/// Resuelve la colisi�n de una entidad con el terreno.
		/// </summary>
		/// <param name="entity">Enitdad.</param>
		/// <param name="transform">Transform de la entidad.</param>
		/// <param name="info">Informaci�n de la colisi�n.</param>
		/// <param name="triangleNormal">Normal del tri�ngulo del terreno con el que se ha colisionado.</param>
		/// <param name="delta">Delta.</param>
		void resolveTerrainCollision(PhysicsComponent* entity, Transform* transform, const Collision::SAT_CollisionInfo& info, const Vector3f& triangleNormal, deltaTime_t delta);

		/// <summary>
		/// Comprueba la colisi�n de una entidad con el terreno.
		/// </summary>
		/// <param name="entity">Entidad.</param>
		/// <param name="transform">Transform de la entidad.</param>
		/// <param name="delta">Delta.</param>
		/// <param name="box">AABB del terreno.</param>
		void resolveTerrainCollisionAABB(PhysicsComponent* entity, Transform* transform, deltaTime_t delta, const CollisionBox& box);

		/// <summary>
		/// Resuelve la colisi�n de una entidad con el terreno, afectando a su rotaci�n.
		/// </summary>
		/// <param name="entity">Entidad.</param>
		/// <param name="transform">Transform de la entidad.</param>
		/// <param name="info">Informaci�n de la colisi�n.</param>
		/// <param name="triangleNormal">Normal del tri�ngulo del terreno con el que se ha colisionado.</param>
		/// <param name="delta">Delta.</param>
		void resolveTerrainCollisionRotation(PhysicsComponent* entity, Transform* transform, const Collision::SAT_CollisionInfo& info, const Vector3f& triangleNormal, deltaTime_t delta);

	};

}
