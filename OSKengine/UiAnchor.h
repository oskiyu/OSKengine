#pragma once

#include "OSKmacros.h"

namespace OSK {

	/// <summary>
	/// Punto de anclaje de un elemento de UI respecto a su padre.
	/// Por defecto: TOP_LEFT.
	/// </summary>
	enum class UiAnchor {

		/// <summary>
		/// Posición respecto a la esquina superior izquierda de su padre.
		/// Dirección:
		/// X: hacia la derecha.
		/// Y: hacia abajo.
		/// </summary>
		TOP_LEFT,

		/// <summary>
		/// Posición respecto a la izquierda de su padre.
		/// Dirección:
		/// X: hacia la derecha.
		/// Y: hacia abajo.
		/// </summary>
		LEFT,

		/// <summary>
		/// Posición respecto a la esquina inferior izquierda de su padre.
		/// Dirección:
		/// X: hacia la derecha.
		/// Y: hacia arriba.
		/// </summary>
		BOTTOM_LEFT,

		/// <summary>
		/// Posición respecto al borde inferior de su padre.
		/// Dirección:
		/// X: hacia la derecha.
		/// Y: hacia arriba.
		/// </summary>
		BOTTOM,

		/// <summary>
		/// Posición respecto a la esquina inferior derecha de su padre.
		/// Dirección:
		/// X: hacia la izquierda.
		/// Y: hacia arriba.
		/// </summary>
		BOTTOM_RIGHT,

		/// <summary>
		/// Posición respecto al borde derecho de su padre.
		/// Dirección:
		/// X: hacia la izquierda.
		/// Y: hacia abajo.
		/// </summary>
		RIGHT,

		/// <summary>
		/// Posición respecto al borde derecho de su padre.
		/// Dirección:
		/// X: hacia la izquierda.
		/// Y: hacia abajo.
		/// </summary>
		TOP_RIGHT,

		/// <summary>
		/// Posición respecto al borde superior de su padre.
		/// Dirección:
		/// X: hacia la derecha.
		/// Y: hacia abajo.
		/// </summary>
		TOP,

		/// <summary>
		/// Posición respecto al centro de su padre.
		/// Dirección:
		/// X: hacia la derecha.
		/// Y: hacia abajo.
		/// </summary>
		CENTER

	};

}