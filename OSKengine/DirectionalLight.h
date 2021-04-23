#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Color.h"

#include <glm.hpp>

namespace OSK {

	/// <summary>
	/// Representa una luz direccional.
	/// </summary>
	struct OSKAPI_CALL DirectionalLight {

		/// <summary>
		/// Dirección de la luz.
		/// </summary>
		alignas(16) Vector3 Direction;

		/// <summary>
		/// Color de la luz.
		/// </summary>
		alignas(16) Color Color;

		/// <summary>
		/// Intensidad de la luz. 
		/// (0.0 - 1.0)
		/// </summary>
		alignas(16) float Intensity;

	};

}