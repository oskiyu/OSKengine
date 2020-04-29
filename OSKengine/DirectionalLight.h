#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Color.h"

#include <glm.hpp>

namespace OSK {

	//Representa una luz direccional.
	struct DirectionalLight {

		//Dirección de la luz.
		Vector3 Direction;

		//Color de la luz.
		Color Color;

		//Intensidad de la luz.
		//(0.0 - 1.0)
		float_t Intensity;

	};

}