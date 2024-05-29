#pragma once

#include "NumericTypes.h"

namespace OSK {

	/// @brief Tipo de dato para identificar un
	/// UUID de una interfaz.
	using TInterfaceUuid = UIndex32;

}

/// @brief Establece el Interface UUID para la clase en la
/// que se escriba este macro.
/// @param uuid Identificador.
#define OSK_DEFINE_IUUID(uuid) static TInterfaceUuid GetInterfaceUuid() { return uuid; }

/// @brief Obtiene el Interface UUID de la clase indicada.
/// @param interfaceClass Clase a comprobar.
#define OSK_IUUID(interfaceClass) (interfaceClass ::GetInterfaceUuid())
