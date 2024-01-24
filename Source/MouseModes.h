#pragma once

namespace OSK::IO {

	/// <summary>
	/// Comportamiento del rat�n.
	/// </summary>
	enum class MouseReturnMode {

		/// <summary>
		/// El rat�n se mueve libremente.
		/// </summary>
		FREE,

		/// <summary>
		/// El rat�n siempre est� en el centro de la pantalla.
		/// </summary>
		ALWAYS_RETURN

	};

	/// <summary>
	/// Modos de aceleraci�n por software que puede tener el rat�n.
	/// </summary>
	enum class MouseMotionMode {

		/// <summary>
		/// Aceleraci�n desactivada.
		/// </summary>
		RAW,

		/// <summary>
		/// Aceleraci�n activada.
		/// </summary>
		ACCELERATED

	};

}
