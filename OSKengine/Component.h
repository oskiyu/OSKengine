#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include <bitset>

namespace OSK {

	/// <summary>
	/// ID de un tipo de componente.
	/// </summary>
	typedef uint16_t ComponentType;

	/// <summary>
	/// Máximo de tipos de componentes.
	/// </summary>
	constexpr ComponentType MAX_COMPONENTS = 256;

	/// <summary>
	/// Signature: componentes que contiene una entidad en concreto.
	/// </summary>
	typedef std::bitset<MAX_COMPONENTS> Signature;


	/// <summary>
	/// Componente que puede añadirse a una entidad.
	/// Cada entidad puede tener un único componente de cada tipo.
	/// Permite añadir funcionalidad a la entidad.
	/// </summary>
	class Component {

	public:

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Component() = default;

		/// <summary>
		/// Función que se ejecuta al crearse el componente.
		/// </summary>
		virtual void OnCreate() {}

		/// <summary>
		/// Función que se ejecuta al quitarse el componente.
		/// </summary>
		virtual void OnRemove() {}

		/// <summary>
		/// Tipo de componente.
		/// </summary>
		ComponentType Type;

	};

}
