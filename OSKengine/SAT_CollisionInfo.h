#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK::Collision {

	//Información de una colisión SAT.
	struct OSKAPI_CALL SAT_CollisionInfo {
		//True si están colisionando.
		bool IsColliding = false;
		//Eje de la colisión.
		Vector3f Axis;
		//MTV de A.
		Vector3f MinimunTranslationVector;
		//Punto de colisión respecto a A.
		Vector3f PointA;
		//Punto de colisión respecto a B.
		Vector3f PointB;
	};

}
