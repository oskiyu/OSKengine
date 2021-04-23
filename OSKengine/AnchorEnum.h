#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Tipos de anclaje 2D.
	/// Establece el punto de referencia de un sprite al renderizarlo.
	/// </summary>
	enum class OSKAPI_CALL Anchor {

		/// <summary>
		/// (Por defecto).
		/// (0, 0) está en la esquina superior izquierda.
		/// </summary>
		TOP_LEFT,

		/// <summary>
		/// (0, 0) está en la esquina superior derecha.
		/// </summary>
		TOP_RIGHT,

		/// <summary>
		/// (0, 0) está en la esquina inferior izquierda.
		/// </summary>
		BOTTOM_LEFT,

		/// <summary>
		/// (0, 0) está en la esquina inferior derecha.
		/// </summary>
		BOTTOM_RIGHT,

		/// <summary>
		/// (0, 0) está en el centro.
		/// </summary>
		CENTER,

		/// <summary>
		/// (0, 0) está en el límite superior (en el centro).
		/// </summary>
		CENTER_TOP,

		/// <summary>
		/// (0, 0) está en el límite inferior (en el centro).
		/// </summary>
		CENTER_BOTTOM,

		/// <summary>
		/// (0, 0) está en la izquierda.
		/// </summary>
		CENTER_LEFT,

		/// <summary>
		/// (0, 0) está en la derecha.
		/// </summary>
		CENTER_RIGHT

	};

}
