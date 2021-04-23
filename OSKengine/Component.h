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
	/// M�ximo de tipos de componentes.
	/// </summary>
	constexpr ComponentType MAX_COMPONENTS = 256;

	/// <summary>
	/// Signature: componentes que contiene una entidad en concreto.
	/// </summary>
	typedef std::bitset<MAX_COMPONENTS> Signature;


	/// <summary>
	/// Componente que puede a�adirse a una entidad.
	/// Cada entidad puede tener un �nico componente de cada tipo.
	/// Permite a�adir funcionalidad a la entidad.
	/// </summary>
	class Component {

	public:

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~Component() = default;

		/// <summary>
		/// Funci�n que se ejecuta al crearse el componente.
		/// </summary>
		virtual void OnCreate() {}

		/// <summary>
		/// Funci�n que se ejecuta al quitarse el componente.
		/// </summary>
		virtual void OnRemove() {}

		/// <summary>
		/// Tipo de componente.
		/// </summary>
		ComponentType Type;

	};

}
