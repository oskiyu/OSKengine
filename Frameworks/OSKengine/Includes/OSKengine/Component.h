#pragma once

#include "OSKmacros.h"
#include "ConstexprBitSet.hpp"

namespace OSK::ECS {

	/// <summary>
	/// Identificador del tipo de componente.
	/// </summary>
	using ComponentType = uint16_t;

	/// <summary>
	/// Número máximo de componentes que se pueden registrar
	/// (y que, por lo tanto, se pueden utilizar).
	/// </summary>
	constexpr ComponentType MAX_COMPONENT_TYPES = 256;

	/// <summary>
	/// Identificador único para un componente.
	/// 
	/// Un componente es una estructura que almacena datos específicos para uno o varios sistemas.
	/// Los componentes no ejecutan lógica alguna, y sólo son contenedores de información 
	/// relacionada con un GameObject.
	/// 
	/// @warning Para que una clase pueda ser usada como componente, debe tener implementado OSK_COMPONENT(className).
	/// @warning Este debe ser un identificador único: no pueden haber dos tipos de componentes con el mismo className.
	/// 
	/// @note Cada GameObject sólo puede tener un componente de cada tipo.
	/// </summary>
	using ComponentIndex = TSize;

	/// <summary>
	/// Un signature permite saber qué componentes contiene un GameObject.
	/// Es un bitmap, donde el bit en la posición 'X' indicará si el
	/// GameObject tiene un componente del tipo (ComponentType == X).
	/// 
	/// Los sistemas también tienen su propio signature, que indica qué componentes
	/// necesita tener un GameObject para ser procesado por ese sistema.
	/// </summary>
	using Signature = ConstexprBitSet<MAX_COMPONENT_TYPES>;

#ifndef OSK_COMPONENT
#define OSK_COMPONENT(className) const static inline std::string GetComponentTypeName() { return className; }
#endif

}
