#pragma once

#include "OSKmacros.h"

namespace OSK::ECS {

	/// <summary>
	/// Identificador �nico para un Game Object.
	/// 
	/// Un game object es una entidad que vive en el juego.
	/// Puede tener asignado cero, uno o varios componentes.
	/// 
	/// Un componente es una estructura que almacena datos espec�ficos para uno o varios sistemas.
	/// Los componentes no ejecutan l�gica alguna, y s�lo son contenedores de informaci�n 
	/// relacionada con un GameObject.
	/// 
	/// @warning Cada GameObject s�lo puede tener un componente de cada tipo.
	/// </summary>
	using GameObjectIndex = TIndex;

	/// <summary>
	/// El id no identifica a ning�n objeto v�lido.
	/// </summary>
	constexpr GameObjectIndex EMPTY_GAME_OBJECT = 0;

}
