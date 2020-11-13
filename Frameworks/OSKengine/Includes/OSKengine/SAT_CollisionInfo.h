#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK::Collision {

	//Informaci�n de una colisi�n SAT.
	struct OSKAPI_CALL SAT_CollisionInfo {
		//True si est�n colisionando.
		bool IsColliding = false;
		//Eje de la colisi�n.
		Vector3f Axis;
		//MTV de A.
		Vector3f MinimunTranslationVector;
		//Punto de colisi�n respecto a A.
		Vector3f PointA;
		//Punto de colisi�n respecto a B.
		Vector3f PointB;
	};

}
