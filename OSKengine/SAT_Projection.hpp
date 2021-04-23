#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK::Collision {

	/// <summary>
	/// Representa la proyecci�n de un SAT_Collider sobre un eje.
	/// El segmento de recta de un eje sobre el que se encuentra el colisionador.
	/// </summary>
	class OSKAPI_CALL SAT_Projection {

	public:

		/// <summary>
		/// Inicia los valores de una proyecci�n.
		/// </summary>
		/// <param name="min">Principio de la proyecci�n.</param>
		/// <param name="max">Final de la proyecci�n.</param>
		SAT_Projection(float min, float max);

		/// <summary>
		/// Devuelve true si una proyecci�n toca a otra.
		/// Solo debe usarse con proyecciones sobre un mismo eje.
		/// </summary>
		/// <param name="proj">Otra proyecci�n.</param>
		/// <returns>True si las dos proyecciones se tocan.</returns>
		bool Overlaps(const SAT_Projection& proj) const;

		/// <summary>
		/// Devuelve la profundidad del overlap de dos proyecciones.
		/// Solo debe usarse con proyecciones sobre un mismo eje.
		/// </summary>
		/// <param name="proj">Otra proyecci�n.</param>
		/// <returns>Overlap de las proyecciones (0.0f si no se tocan).</returns>
		float GetOverlap(const SAT_Projection& proj) const;

	private:

		/// <summary>
		/// Inicio de la proyecci�n.
		/// </summary>
		float min;

		/// <summary>
		/// Fin de la proyecci�n.
		/// </summary>
		float max;

	};

}
