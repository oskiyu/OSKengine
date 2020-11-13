#pragma once

#include "OSKmacros.h"
#include "OSKtypes.h"

namespace OSK {

	//Representa la movilidad de una entidad física.
	enum class OSKAPI_CALL PhysicalEntityMobilityType {
		//No se puede mover.
		STATIC,
		//Se puede mover.
		MOVABLE
	};


	//Representa qué reacción tendrá una entidad ante una colisión.
	enum OSKAPI_CALL PhysicalEntityResponseBitFlags : bitFlags_t {
		//La entidad se moverá siguiendo su MTV.
		PHYSICAL_ENTITY_RESPONSE_MOVE_MTV = 1 << 0,
		//La entidad cambiará de velocidad tras el choque.
		PHYSICAL_ENTITY_RESPONSE_ACCEL = 1 << 1,
		//La entidad puede rotar tras el choque.
		PHYSICAL_ENTITY_RESPONSE_ROTATE = 1 << 2
	};

	//Representa qué reacción provocará una entidad a otra ante una colisión.
	enum OSKAPI_CALL PhysicalEntityInteractionBitFlags : bitFlags_t {
		//La otra entidad recibirá un cambio de velocidad.
		PHYSICAL_ENTITY_INTERACTION_GIVE_ACCEL = 1 << 0,
		//La otra entidad recibirá un cambio de velocidad angular y rotación.
		PHYSICAL_ENTITY_INTERACTION_GIVE_ROTATION = 1 << 1
	};


	//Representa qué simulará el motor de físicas.
	enum OSKAPI_CALL PhysicalSceneSimulateBitFlags : bitFlags_t {
		//Aplicará la velocidad de las entidades.
		PHYSICAL_SCENE_SIMULATE_VELOCITY = 1 << 0,
		//Aplicará aceleración a las entidades.
		PHYSICAL_SCENE_SIMULATE_ACCEL = 1 << 1,
		//Aplicará rotación a las entidades.
		PHYSICAL_SCENE_SIMULATE_ROTATION = 1 << 2
	};

	//Representa qué hará el motor de físicas cuando se produzca una colisión.
	enum OSKAPI_CALL PhysicalSceneResolveBitFlags : bitFlags_t {
		//Cambiará la velocidad de las entidades.
		PHYSICAL_SCENE_RESOLVE_ACCEL = 1 << 0,
		//Cambiará la rotación y velocidad angular de las entidades.
		PHYSICAL_SCENE_RESOLVE_ROTATION = 1 << 1
	};

	//Indica qué hará el motor de físicas al detectar una colisión con el terreno.
	enum class OSKAPI_CALL PhysicalSceneTerrainResolveType {
		//No hace nada.
		DO_NOTHING,
		//Resuelve una colisión compleja, incluyendo rotación.
		RESOLVE_DETAILED,
		//Simplemente cambia la posición Y de la entidad.
		CHANGE_HEIGHT_ONLY
	};

}
