#pragma once

#include "Macros.h"

#include <cstdint>

namespace OSK::NET {

	/// <summary>
	/// ID de un cliente conectado a un servidor.
	/// </summary>
	typedef unsigned int clientID_t;

	/// <summary>
	/// Byte.
	/// </summary>
	typedef unsigned char byte_t;

	/// <summary>
	/// Tipo que se usa para escribir la longitud de un string en un mensaje.
	/// </summary>
	typedef uint16_t stringLength_t;

}
