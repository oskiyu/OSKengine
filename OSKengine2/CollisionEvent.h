#pragma once

#include "Event.h"
#include "GameObject.h"
#include "DetailedCollisionInfo.h"

#include <string>

namespace OSK::ECS {

	/// @brief Evento que representa una colisi�n
	/// entre dos entidades.
	struct CollisionEvent {

		OSK_EVENT("OSK::CollisionEvent");

		/// @brief Primera entidad de la colisi�n.
		GameObjectIndex firstEntity = EMPTY_GAME_OBJECT;
		/// @brief Segunda entidad de la colisi�n.
		GameObjectIndex secondEntity = EMPTY_GAME_OBJECT;

		/// @brief Informaci�n sobre la colisi�n.
		COLLISION::DetailedCollisionInfo collisionInfo;

	};

}
