#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Botones del ratón.
	/// </summary>
	enum class OSKAPI_CALL ButtonCode {

		/// <summary>
		/// Botón extra #1.
		/// </summary>
		BUTTON_1,

		/// <summary>
		/// Botón extra #2.
		/// </summary>
		BUTTON_2,

		/// <summary>
		/// Botón extra #3.
		/// </summary>
		BUTTON_3,

		/// <summary>
		/// Botón extra #4.
		/// </summary>
		BUTTON_4,

		/// <summary>
		/// Botón extra #5.
		/// </summary>
		BUTTON_5,

		/// <summary>
		/// Botón extra #6.
		/// </summary>
		BUTTON_6,

		/// <summary>
		/// Botón extra #7.
		/// </summary>
		BUTTON_7,

		/// <summary>
		/// Botón izquierdo.
		/// </summary>
		BUTTON_LEFT,

		/// <summary>
		/// Botón derecho.
		/// </summary>
		BUTTON_RIGHT,

		/// <summary>
		/// Botón de la rueda.
		/// </summary>
		BUTTON_MIDDLE

	};

}