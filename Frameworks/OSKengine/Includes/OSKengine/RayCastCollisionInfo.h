#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Informaci�n sobre la intersecci�n de un rayo con un SAT_Collider.
	/// </summary>
	struct OSKAPI_CALL RayCastCollisionInfo {

		/// <summary>
		/// True si lo intersecta.
		/// </summary>
		bool IsColliding = false;

		/// <summary>
		/// Distancia del punto de intersecci�n desde el origen del rayo.
		/// </summary>
		float DistanceFromOrigin = { 0.0f };

	};

}