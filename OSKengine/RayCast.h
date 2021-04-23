#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "SAT_Collider.h"
#include "CollisionBox.h"
#include "RayCastCollisionInfo.h"

#include <gtc/matrix_access.hpp>

namespace OSK{

	/// <summary>
	/// Clase est�tica para hacer raycasts.
	/// </summary>
	class OSKAPI_CALL RayCast {

	public:

		/// <summary>
		/// Lanza un rayo desde el punto de origen y comprueba si toca el collider dado.
		/// </summary>
		/// <param name="origin">Posici�n origen del rayo.</param>
		/// <param name="direction">Direcci�n del rayo.</param>
		/// <param name="sat">Collider a comprobar.</param>
		/// <returns>Informaci�n del raycast.</returns>
		static RayCastCollisionInfo CastRay(const Vector3f& origin, Vector3f direction, const Collision::SAT_Collider& sat);

	};

}
