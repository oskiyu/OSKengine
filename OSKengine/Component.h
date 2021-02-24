#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include <bitset>

namespace OSK {

	//ID de un tipo de componente.
	typedef uint16_t ComponentType;

	//Máximo de tipos de componentes.
	constexpr ComponentType MAX_COMPONENTS = 256;

	//Signature: componentes de una entidad.
	typedef std::bitset<MAX_COMPONENTS> Signature;


	//Componente de una entidad.
	class Component {

	public:

		virtual ~Component() = default;

		virtual void OnCreate() {}
		virtual void OnRemove() {}

		//Tipo de componente.
		ComponentType Type;

	};

}
