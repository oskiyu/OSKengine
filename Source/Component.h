#pragma once

#include "NumericTypes.h"
#include "ConstexprBitSet.hpp"

namespace OSK::ECS {

	/// @brief Identificador único para un componente.
	/// 
	/// Un componente es una estructura que almacena datos específicos para uno o varios sistemas.
	/// Los componentes no ejecutan lógica alguna, y sólo son contenedores de información 
	/// relacionada con un GameObject.
	/// 
	/// @warning Para que una clase pueda ser usada como componente, debe tener implementado OSK_COMPONENT(className).
	/// @warning Este debe ser un identificador único: no pueden haber dos tipos de componentes con el mismo className.
	/// 
	/// @note Cada GameObject sólo puede tener un componente de cada tipo.
	using ComponentType = uint16_t;

	/// @brief Número máximo de tipos de componentes que se pueden registrar
	/// (y que, por lo tanto, se pueden utilizar).
	constexpr ComponentType MAX_COMPONENT_TYPES = 256;
		
	/// @brief Índice que ocupa un componente en un contenedor.
	using ComponentIndex = UIndex64;

	/// @brief Un signature permite saber qué componentes contiene un GameObject.
	/// Es un bitset, donde el bit en la posición 'X' indicará si el
	/// GameObject tiene un componente del tipo @code (ComponentType == X) @endcode .
	/// 
	/// Los sistemas también tienen su propio signature, que indica qué componentes
	/// necesita tener un GameObject para ser procesado por ese sistema.
	using Signature = ConstexprBitSet<MAX_COMPONENT_TYPES>;


	/// @brief Concepto que nos permite detectar si una clase cumple con las condiciones
	/// para ser un componente.
	template<typename TComponent>
	concept IsEcsComponent = requires (TComponent) {
		{TComponent::GetComponentTypeName()};
	};

#ifndef OSK_COMPONENT
#define OSK_COMPONENT(className) constexpr static inline std::string_view GetComponentTypeName() { return className; }
#endif

}
