#pragma once

#include "OSKmacros.h"
#include "ConstexprBitSet.hpp"

namespace OSK::ECS {

	/// <summary>
	/// Identificador del tipo de componente.
	/// </summary>
	using ComponentType = uint16_t;

	/// <summary>
	/// N�mero m�ximo de componentes que se pueden registrar
	/// (y que, por lo tanto, se pueden utilizar).
	/// </summary>
	constexpr ComponentType MAX_COMPONENT_TYPES = 256;

	/// <summary>
	/// Identificador �nico para un componente.
	/// 
	/// Un componente es una estructura que almacena datos espec�ficos para uno o varios sistemas.
	/// Los componentes no ejecutan l�gica alguna, y s�lo son contenedores de informaci�n 
	/// relacionada con un GameObject.
	/// 
	/// @warning Para que una clase pueda ser usada como componente, debe tener implementado OSK_COMPONENT(className).
	/// @warning Este debe ser un identificador �nico: no pueden haber dos tipos de componentes con el mismo className.
	/// 
	/// @note Cada GameObject s�lo puede tener un componente de cada tipo.
	/// </summary>
	using ComponentIndex = TSize;

	/// <summary>
	/// Un signature permite saber qu� componentes contiene un GameObject.
	/// Es un bitmap, donde el bit en la posici�n 'X' indicar� si el
	/// GameObject tiene un componente del tipo (ComponentType == X).
	/// 
	/// Los sistemas tambi�n tienen su propio signature, que indica qu� componentes
	/// necesita tener un GameObject para ser procesado por ese sistema.
	/// </summary>
	using Signature = ConstexprBitSet<MAX_COMPONENT_TYPES>;

#ifndef OSK_COMPONENT
#define OSK_COMPONENT(className) const static inline std::string GetComponentTypeName() { return className; }
#endif

}
