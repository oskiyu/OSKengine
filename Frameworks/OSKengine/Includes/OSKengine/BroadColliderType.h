#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	/// <summary>
	/// Tipos de BroadCollider.
	/// </summary>
	enum class OSKAPI_CALL BroadColliderType {

		/// <summary>
		/// Axis oriented box.
		/// </summary>
		BOX_AABB,

		/// <summary>
		/// Esfera.
		/// </summary>
		SPHERE

	};

}