#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Color.h"

#include <glm.hpp>

namespace OSK {

	//Representa una luz direccional.
	struct OSKAPI_CALL DirectionalLight {

		//Dirección de la luz.
		alignas(16) Vector3 Direction;

		//Color de la luz.
		alignas(16) Color Color;

		//Intensidad de la luz.
		//(0.0 - 1.0)
		alignas(16) float_t Intensity;

	};

}