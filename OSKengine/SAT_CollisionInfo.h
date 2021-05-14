#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK::Collision {

	/// <summary>
	/// Información detallada de una colisión SAT.
	/// </summary>
	struct OSKAPI_CALL SAT_CollisionInfo {
		
		/// <summary>
		/// True si los dos colisionadores están colisionando.
		/// </summary>
		bool isColliding = false;

		/// <summary>
		/// Eje de la colisión.
		/// </summary>
		Vector3f axis;
		
		/// <summary>
		/// Vector mínimo de translación de A sobre B:
		/// la distancia que A debería recorrer para no colisionar sobre B.
		/// </summary>
		Vector3f minimunTranslationVector;

		/// <summary>
		/// Punto de colisión (espacio local A).
		/// </summary>
		Vector3f pointA;

		/// <summary>
		/// Punto de colisión (espacio local B).
		/// </summary>
		Vector3f pointB;

	};

}
