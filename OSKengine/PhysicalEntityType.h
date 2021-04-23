#pragma once

#include "OSKmacros.h"
#include "OSKtypes.h"

namespace OSK {

	/// <summary>
	/// Representa la movilidad de una entidad física.
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
	/// Representa qué reacción tendrá una entidad ante una colisión.
	/// </summary>
	enum OSKAPI_CALL PhysicalEntityResponseBitFlags : bitFlags_t {
		
		/// <summary>
		/// La entidad se moverá siguiendo su MTV.
		/// </summary>
		PHYSICAL_ENTITY_RESPONSE_MOVE_MTV = 1 << 0,
		
		/// <summary>
		/// La entidad cambiará de velocidad tras el choque.
		/// </summary>
		PHYSICAL_ENTITY_RESPONSE_ACCEL = 1 << 1,

		/// <summary>
		/// La entidad puede rotar tras el choque.
		/// </summary>
		PHYSICAL_ENTITY_RESPONSE_ROTATE = 1 << 2

	};

	/// <summary>
	/// Representa qué reacción provocará una entidad a otra ante una colisión.
	/// </summary>
	enum OSKAPI_CALL PhysicalEntityInteractionBitFlags : bitFlags_t {

		/// <summary>
		/// La otra entidad recibirá un cambio de velocidad.
		/// </summary>
		PHYSICAL_ENTITY_INTERACTION_GIVE_ACCEL = 1 << 0,

		/// <summary>
		/// La otra entidad recibirá un cambio de velocidad angular y rotación.
		/// </summary>
		PHYSICAL_ENTITY_INTERACTION_GIVE_ROTATION = 1 << 1

	};

	/// <summary>
	/// Representa qué simulará el motor de físicas.
	/// </summary>
	enum OSKAPI_CALL PhysicalSceneSimulateBitFlags : bitFlags_t {

		/// <summary>
		/// Aplicará la velocidad de las entidades.
		/// </summary>
		PHYSICAL_SCENE_SIMULATE_VELOCITY = 1 << 0,

		/// <summary>
		/// Aplicará aceleración a las entidades.
		/// </summary>
		PHYSICAL_SCENE_SIMULATE_ACCEL = 1 << 1,

		/// <summary>
		/// Aplicará rotación a las entidades.
		/// </summary>
		PHYSICAL_SCENE_SIMULATE_ROTATION = 1 << 2

	};

	/// <summary>
	/// Representa qué hará el motor de físicas cuando se produzca una colisión.
	/// </summary>
	enum OSKAPI_CALL PhysicalSceneResolveBitFlags : bitFlags_t {

		/// <summary>
		/// Cambiará la velocidad de las entidades.
		/// </summary>
		PHYSICAL_SCENE_RESOLVE_ACCEL = 1 << 0,

		/// <summary>
		/// Cambiará la rotación y velocidad angular de las entidades.
		/// </summary>
		PHYSICAL_SCENE_RESOLVE_ROTATION = 1 << 1

	};

	/// <summary>
	/// Indica qué hará el motor de físicas al detectar una colisión con el terreno.
	/// </summary>
	enum class OSKAPI_CALL PhysicalSceneTerrainResolveType {
		
		/// <summary>
		/// No hace nada.
		/// </summary>
		DO_NOTHING,

		/// <summary>
		/// Resuelve una colisión compleja, incluyendo rotación.
		/// </summary>
		RESOLVE_DETAILED,

		/// <summary>
		/// Simplemente cambia la posición Y de la entidad.
		/// </summary>
		CHANGE_HEIGHT_ONLY

	};

}
