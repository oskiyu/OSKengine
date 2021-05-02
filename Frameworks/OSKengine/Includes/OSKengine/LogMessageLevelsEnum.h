#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"

namespace OSK {

	/// <summary>
	/// Tipos de mensajes de Log().
	/// </summary>
	enum class OSKAPI_CALL LogMessageLevels {

		/// <summary>
		/// Mensaje que simplemente muestra informaci�n.
		/// </summary>
		INFO,

		/// <summary>
		/// Aviso.
		/// </summary>
		WARNING,

		/// <summary>
		/// Error recuperable.
		/// </summary>
		BAD_ERROR,

		/// <summary>
		/// Error insalvable.
		/// </summary>
		CRITICAL_ERROR,

		/// <summary>
		/// Comando de Log().
		/// </summary>
		COMMAND,

		/// <summary>
		/// Informaci�n de debug.
		/// </summary>
		DEBUG,

		/// <summary>
		/// Sin un tipo en concreto.
		/// </summary>
		NO

	};

}

