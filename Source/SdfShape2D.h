#pragma once

namespace OSK::GRAPHICS {

	/// @brief Tipos de formas para un elemento SDF.
	enum class SdfShape2D : int {

		/// @brief Rectángulo (por defecto).
		/// Cubre todo el área renderizada
		/// por el quad.
		RECTANGLE,

		/// @brief Círculo.
		CIRCLE,

	};

}
