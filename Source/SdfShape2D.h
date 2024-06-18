#pragma once

namespace OSK::GRAPHICS {

	/// @brief Tipos de formas para un elemento SDF.
	enum class SdfShape2D : int {

		/// @brief Rect�ngulo (por defecto).
		/// Cubre todo el �rea renderizada
		/// por el quad.
		RECTANGLE,

		/// @brief C�rculo.
		CIRCLE,

	};

}
