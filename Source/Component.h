#pragma once

#include "NumericTypes.h"
#include "ConstexprBitSet.hpp"

namespace OSK::ECS {

	/// @brief Identificador �nico para un componente.
	/// 
	/// Un componente es una estructura que almacena datos espec�ficos para uno o varios sistemas.
	/// Los componentes no ejecutan l�gica alguna, y s�lo son contenedores de informaci�n 
	/// relacionada con un GameObject.
	/// 
	/// @warning Para que una clase pueda ser usada como componente, debe tener implementado OSK_COMPONENT(className).
	/// @warning Este debe ser un identificador �nico: no pueden haber dos tipos de componentes con el mismo className.
	/// 
	/// @note Cada GameObject s�lo puede tener un componente de cada tipo.
	using ComponentType = uint16_t;

	/// @brief N�mero m�ximo de tipos de componentes que se pueden registrar
	/// (y que, por lo tanto, se pueden utilizar).
	constexpr ComponentType MAX_COMPONENT_TYPES = 256;
		
	/// @brief �ndice que ocupa un componente en un contenedor.
	using ComponentIndex = UIndex64;

	/// @brief Un signature permite saber qu� componentes contiene un GameObject.
	/// Es un bitset, donde el bit en la posici�n 'X' indicar� si el
	/// GameObject tiene un componente del tipo @code (ComponentType == X) @endcode .
	/// 
	/// Los sistemas tambi�n tienen su propio signature, que indica qu� componentes
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
