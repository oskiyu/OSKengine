#pragma once

namespace OSK::IO {

	/// <summary>
	/// Comportamiento del ratón.
	/// </summary>
	enum class MouseReturnMode {

		/// <summary>
		/// El ratón se mueve libremente.
		/// </summary>
		FREE,

		/// <summary>
		/// El ratón siempre está en el centro de la pantalla.
		/// </summary>
		ALWAYS_RETURN

	};

	/// <summary>
	/// Modos de aceleración por software que puede tener el ratón.
	/// </summary>
	enum class MouseMotionMode {

		/// <summary>
		/// Aceleración desactivada.
		/// </summary>
		RAW,

		/// <summary>
		/// Aceleración activada.
		/// </summary>
		ACCELERATED

	};

}
