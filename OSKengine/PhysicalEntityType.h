#pragma once

#include "OSKmacros.h"
#include "OSKtypes.h"

namespace OSK {

	//Representa la movilidad de una entidad f�sica.
	enum class OSKAPI_CALL PhysicalEntityMobilityType {
		//No se puede mover.
		STATIC,
		//Se puede mover.
		MOVABLE
	};


	//Representa qu� reacci�n tendr� una entidad ante una colisi�n.
	enum OSKAPI_CALL PhysicalEntityResponseBitFlags : bitFlags_t {
		//La entidad se mover� siguiendo su MTV.
		PHYSICAL_ENTITY_RESPONSE_MOVE_MTV = 1 << 0,
		//La entidad cambiar� de velocidad tras el choque.
		PHYSICAL_ENTITY_RESPONSE_ACCEL = 1 << 1,
		//La entidad puede rotar tras el choque.
		PHYSICAL_ENTITY_RESPONSE_ROTATE = 1 << 2
	};

	//Representa qu� reacci�n provocar� una entidad a otra ante una colisi�n.
	enum OSKAPI_CALL PhysicalEntityInteractionBitFlags : bitFlags_t {
		//La otra entidad recibir� un cambio de velocidad.
		PHYSICAL_ENTITY_INTERACTION_GIVE_ACCEL = 1 << 0,
		//La otra entidad recibir� un cambio de velocidad angular y rotaci�n.
		PHYSICAL_ENTITY_INTERACTION_GIVE_ROTATION = 1 << 1
	};


	//Representa qu� simular� el motor de f�sicas.
	enum OSKAPI_CALL PhysicalSceneSimulateBitFlags : bitFlags_t {
		//Aplicar� la velocidad de las entidades.
		PHYSICAL_SCENE_SIMULATE_VELOCITY = 1 << 0,
		//Aplicar� aceleraci�n a las entidades.
		PHYSICAL_SCENE_SIMULATE_ACCEL = 1 << 1,
		//Aplicar� rotaci�n a las entidades.
		PHYSICAL_SCENE_SIMULATE_ROTATION = 1 << 2
	};

	//Representa qu� har� el motor de f�sicas cuando se produzca una colisi�n.
	enum OSKAPI_CALL PhysicalSceneResolveBitFlags : bitFlags_t {
		//Cambiar� la velocidad de las entidades.
		PHYSICAL_SCENE_RESOLVE_ACCEL = 1 << 0,
		//Cambiar� la rotaci�n y velocidad angular de las entidades.
		PHYSICAL_SCENE_RESOLVE_ROTATION = 1 << 1
	};

	//Indica qu� har� el motor de f�sicas al detectar una colisi�n con el terreno.
	enum class OSKAPI_CALL PhysicalSceneTerrainResolveType {
		//No hace nada.
		DO_NOTHING,
		//Resuelve una colisi�n compleja, incluyendo rotaci�n.
		RESOLVE_DETAILED,
		//Simplemente cambia la posici�n Y de la entidad.
		CHANGE_HEIGHT_ONLY
	};

}
