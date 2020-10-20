#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "SAT_Collider.h"
#include "CollisionBox.h"

#include <gtc/matrix_access.hpp>

namespace OSK{

	//Información sobre la intersección de un rayo con un SAT_Collider.
	struct OSKAPI_CALL RayCastCollisionInfo {
		//True si lo intersecta.
		bool IsColliding = false;
		//Distancia del punto de intersección desde el origen del rayo.
		float DistanceFromOrigin = { 0.0f };
	};

	//Clase estática para hacer raycasts.
	class OSKAPI_CALL RayCast {

	public:

		//Lanza un rayo desde el punto de origen y comprueba si toca el collider dado.
		//	<origin>: origen del rayo.
		//	<direction>: dirección del rayo.
		//	<sat>: collider dado.
		static RayCastCollisionInfo CastRay(const Vector3f& origin, Vector3f direction, const Collision::SAT_Collider& sat);

	};

}
