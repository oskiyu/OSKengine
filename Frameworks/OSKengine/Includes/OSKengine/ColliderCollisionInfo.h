#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "SAT_Collider.h"

namespace OSK {

	/// <summary>
	/// Estructura que contiene informaci�n detallada de una colisi�n entre Colliders.
	/// </summary>
	struct OSKAPI_CALL ColliderCollisionInfo {

		/// <summary>
		/// True si hay colisi�n.
		/// </summary>
		bool IsColliding = false;
		
		/// <summary>
		/// True si hay colisi�n entre los BroadColliders.
		/// No tiene por qu� suponer una colisi�n real.
		/// </summary>
		bool IsBroadColliderColliding = false;

		/// <summary>
		/// Primer sat que colisiona.
		/// </summary>
		Collision::SAT_Collider* SAT_1 = nullptr;

		/// <summary>
		/// Segundo sat que colisiona.
		/// </summary>
		Collision::SAT_Collider* SAT_2 = nullptr;

	};

}