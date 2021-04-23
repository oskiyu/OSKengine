#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK::Collision {

	/// <summary>
	/// Informaci�n detallada de una colisi�n SAT.
	/// </summary>
	struct OSKAPI_CALL SAT_CollisionInfo {
		
		/// <summary>
		/// True si los dos colisionadores est�n colisionando.
		/// </summary>
		bool IsColliding = false;

		/// <summary>
		/// Eje de la colisi�n.
		/// </summary>
		Vector3f Axis;
		
		/// <summary>
		/// Vector m�nimo de translaci�n de A sobre B:
		/// la distancia que A deber�a recorrer para no colisionar sobre B.
		/// </summary>
		Vector3f MinimunTranslationVector;

		/// <summary>
		/// Punto de colisi�n (espacio local A).
		/// </summary>
		Vector3f PointA;

		/// <summary>
		/// Punto de colisi�n (espacio local B).
		/// </summary>
		Vector3f PointB;

	};

}
