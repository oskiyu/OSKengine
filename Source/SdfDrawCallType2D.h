#pragma once

namespace OSK::GRAPHICS {

	/// @brief Configuraci�n para elegir
	/// c�mo se establece el color de
	/// un elemento SDF.
	enum class SdfDrawCallContentType2D : int {

		/// @brief Tetxura 2D.
		TEXTURE,

		/// @brief Un �nico color.
		COLOR_FLAT,

		/// @brief Gradiente entre dos colores.
		COLOR_SIMPLE_GRADIENT

	};

}
