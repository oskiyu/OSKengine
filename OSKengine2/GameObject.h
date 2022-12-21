#pragma once

#include "OSKmacros.h"

namespace OSK::ECS {

	/// <summary>
	/// Identificador único para un Game Object.
	/// 
	/// Un game object es una entidad que vive en el juego.
	/// Puede tener asignado cero, uno o varios componentes.
	/// 
	/// Un componente es una estructura que almacena datos específicos para uno o varios sistemas.
	/// Los componentes no ejecutan lógica alguna, y sólo son contenedores de información 
	/// relacionada con un GameObject.
	/// 
	/// @warning Cada GameObject sólo puede tener un componente de cada tipo.
	/// </summary>
	using GameObjectIndex = TIndex;

	/// <summary>
	/// El id no identifica a ningún objeto válido.
	/// </summary>
	constexpr GameObjectIndex EMPTY_GAME_OBJECT = 0;

}
