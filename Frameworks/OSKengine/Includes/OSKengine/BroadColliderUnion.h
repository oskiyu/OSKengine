#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "CollisionBox.h"
#include "CollisionSphere.h"

namespace OSK {

	/// <summary>
	/// Representa un broad collider, que puede ser CollisionBox o CollisionSphere.
	/// </summary>
	union OSKAPI_CALL BroadColliderUnion {

		/// <summary>
		/// Inicia el BroadCollider (vacío).
		/// </summary>
		inline BroadColliderUnion() {
			memset(this, 0, sizeof(*this));
		}

		/// <summary>
		/// Axis oriented box.
		/// </summary>
		CollisionBox Box;

		/// <summary>
		/// Esfera.
		/// </summary>
		CollisionSphere Sphere;

	};

}