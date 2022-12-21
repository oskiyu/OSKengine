#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "SAT_Collider.h"

namespace OSK {

	/// <summary>
	/// Estructura que contiene información detallada de una colisión entre Colliders.
	/// </summary>
	struct OSKAPI_CALL ColliderCollisionInfo {

		/// <summary>
		/// True si hay colisión.
		/// </summary>
		bool isColliding = false;
		
		/// <summary>
		/// True si hay colisión entre los BroadColliders.
		/// No tiene por qué suponer una colisión real.
		/// </summary>
		bool isBroadColliderColliding = false;

		/// <summary>
		/// Primer sat que colisiona.
		/// </summary>
		Collision::SAT_Collider* sat1 = nullptr;

		/// <summary>
		/// Segundo sat que colisiona.
		/// </summary>
		Collision::SAT_Collider* sat2 = nullptr;

	};

}
