#pragma once

#include "OSKmacros.h"
#include "OSKtypes.h"

namespace OSK {

	/// <summary>
	/// Representa la movilidad de una entidad f�sica.
	/// </summary>
	enum class OSKAPI_CALL PhysicalEntityMobilityType {
		
		/// <summary>
		/// No se puede mover.
		/// </summary>
		STATIC,

		/// <summary>
		/// Se puede mover.
		/// </summary>
		MOVABLE

	};

	/// <summary>
	/// Representa qu� reacci�n tendr� una entidad ante una colisi�n.
	/// </summary>
	enum OSKAPI_CALL PhysicalEntityResponseBitFlags : bitFlags_t {
		
		/// <summary>
		/// La entidad se mover� siguiendo su MTV.
		/// </summary>
		PHYSICAL_ENTITY_RESPONSE_MOVE_MTV = 1 << 0,
		
		/// <summary>
		/// La entidad cambiar� de velocidad tras el choque.
		/// </summary>
		PHYSICAL_ENTITY_RESPONSE_ACCEL = 1 << 1,

		/// <summary>
		/// La entidad puede rotar tras el choque.
		/// </summary>
		PHYSICAL_ENTITY_RESPONSE_ROTATE = 1 << 2

	};

	/// <summary>
	/// Representa qu� reacci�n provocar� una entidad a otra ante una colisi�n.
	/// </summary>
	enum OSKAPI_CALL PhysicalEntityInteractionBitFlags : bitFlags_t {

		/// <summary>
		/// La otra entidad recibir� un cambio de velocidad.
		/// </summary>
		PHYSICAL_ENTITY_INTERACTION_GIVE_ACCEL = 1 << 0,

		/// <summary>
		/// La otra entidad recibir� un cambio de velocidad angular y rotaci�n.
		/// </summary>
		PHYSICAL_ENTITY_INTERACTION_GIVE_ROTATION = 1 << 1

	};

	/// <summary>
	/// Representa qu� simular� el motor de f�sicas.
	/// </summary>
	enum OSKAPI_CALL PhysicalSceneSimulateBitFlags : bitFlags_t {

		/// <summary>
		/// Aplicar� la velocidad de las entidades.
		/// </summary>
		PHYSICAL_SCENE_SIMULATE_VELOCITY = 1 << 0,

		/// <summary>
		/// Aplicar� aceleraci�n a las entidades.
		/// </summary>
		PHYSICAL_SCENE_SIMULATE_ACCEL = 1 << 1,

		/// <summary>
		/// Aplicar� rotaci�n a las entidades.
		/// </summary>
		PHYSICAL_SCENE_SIMULATE_ROTATION = 1 << 2

	};

	/// <summary>
	/// Representa qu� har� el motor de f�sicas cuando se produzca una colisi�n.
	/// </summary>
	enum OSKAPI_CALL PhysicalSceneResolveBitFlags : bitFlags_t {

		/// <summary>
		/// Cambiar� la velocidad de las entidades.
		/// </summary>
		PHYSICAL_SCENE_RESOLVE_ACCEL = 1 << 0,

		/// <summary>
		/// Cambiar� la rotaci�n y velocidad angular de las entidades.
		/// </summary>
		PHYSICAL_SCENE_RESOLVE_ROTATION = 1 << 1

	};

	/// <summary>
	/// Indica qu� har� el motor de f�sicas al detectar una colisi�n con el terreno.
	/// </summary>
	enum class OSKAPI_CALL PhysicalSceneTerrainResolveType {
		
		/// <summary>
		/// No hace nada.
		/// </summary>
		DO_NOTHING,

		/// <summary>
		/// Resuelve una colisi�n compleja, incluyendo rotaci�n.
		/// </summary>
		RESOLVE_DETAILED,

		/// <summary>
		/// Simplemente cambia la posici�n Y de la entidad.
		/// </summary>
		CHANGE_HEIGHT_ONLY

	};

}
