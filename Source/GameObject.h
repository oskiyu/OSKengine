#pragma once

#include "OSKmacros.h"
#include "Uuid.h"

#include <format>


namespace OSK::ECS {

	/// @brief Identificador único para un Game Object.
	/// 
	/// Un game object es una entidad que vive en el juego.
	/// Puede tener asignado cero, uno o varios componentes.
	/// 
	/// Un componente es una estructura que almacena datos específicos para uno o varios sistemas.
	/// Los componentes no ejecutan lógica alguna, y sólo son contenedores de información 
	/// relacionada con un GameObject.
	/// 
	/// @warning Cada GameObject sólo puede tener un componente de cada tipo.
	using GameObjectIndex = BaseUuid<class GameObjectIndexTag>;

	/// @brief El ID no identifica a ningún objeto válido.
	constexpr GameObjectIndex EMPTY_GAME_OBJECT = GameObjectIndex::CreateEmpty();

}

OSK_DEFINE_UUID_HASH(OSK::ECS::GameObjectIndex);
OSK_DEFINE_UUID_FORMATTER(OSK::ECS::GameObjectIndex, "GameObjectIndex {}");
