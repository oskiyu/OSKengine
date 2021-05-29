#pragma once

#include "OSKmacros.h"

namespace OSK {

	/// <summary>
	/// Punto de anclaje de un elemento de UI respecto a su padre.
	/// Por defecto: TOP_LEFT.
	/// </summary>
	enum class UiAnchor {

		/// <summary>
		/// Posici�n respecto a la esquina superior izquierda de su padre.
		/// Direcci�n:
		/// X: hacia la derecha.
		/// Y: hacia abajo.
		/// </summary>
		TOP_LEFT,

		/// <summary>
		/// Posici�n respecto a la izquierda de su padre.
		/// Direcci�n:
		/// X: hacia la derecha.
		/// Y: hacia abajo.
		/// </summary>
		LEFT,

		/// <summary>
		/// Posici�n respecto a la esquina inferior izquierda de su padre.
		/// Direcci�n:
		/// X: hacia la derecha.
		/// Y: hacia arriba.
		/// </summary>
		BOTTOM_LEFT,

		/// <summary>
		/// Posici�n respecto al borde inferior de su padre.
		/// Direcci�n:
		/// X: hacia la derecha.
		/// Y: hacia arriba.
		/// </summary>
		BOTTOM,

		/// <summary>
		/// Posici�n respecto a la esquina inferior derecha de su padre.
		/// Direcci�n:
		/// X: hacia la izquierda.
		/// Y: hacia arriba.
		/// </summary>
		BOTTOM_RIGHT,

		/// <summary>
		/// Posici�n respecto al borde derecho de su padre.
		/// Direcci�n:
		/// X: hacia la izquierda.
		/// Y: hacia abajo.
		/// </summary>
		RIGHT,

		/// <summary>
		/// Posici�n respecto al borde derecho de su padre.
		/// Direcci�n:
		/// X: hacia la izquierda.
		/// Y: hacia abajo.
		/// </summary>
		TOP_RIGHT,

		/// <summary>
		/// Posici�n respecto al borde superior de su padre.
		/// Direcci�n:
		/// X: hacia la derecha.
		/// Y: hacia abajo.
		/// </summary>
		TOP,

		/// <summary>
		/// Posici�n respecto al centro de su padre.
		/// Direcci�n:
		/// X: hacia la derecha.
		/// Y: hacia abajo.
		/// </summary>
		CENTER

	};

}