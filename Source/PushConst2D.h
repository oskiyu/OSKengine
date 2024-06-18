#pragma once

#include "Color.hpp"
#include "Vector4.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Información push constant usada en el sistema de renderizado
	/// 2D por defecto.
	/// </summary>
	struct PushConst2D {

		/// <summary>
		/// Matriz de modelo del sprite.
		/// 
		/// Contiene:
		/// - Posición.
		/// - Rotación.
		/// - Escala.
		/// </summary>
		alignas(16) glm::mat4 matrix;

		/// <summary>
		/// Color del sprite.
		/// Se aplicará a todo el sprite.
		/// </summary>
		alignas(16) Color color;

		/// <summary>
		/// Coordenadas de texturas.
		/// 
		/// @note Normalizadas (0.0 - 1.0).
		/// </summary>
		alignas(16) Vector4f texCoords;

		/// @brief Posición del centro del sprite.
		alignas(16) Vector2f centerPosition;

	};

}
