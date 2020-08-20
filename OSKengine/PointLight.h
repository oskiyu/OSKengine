#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Color.h"
#include "Transform.h"

namespace OSK {
	
	//Representa una luz puntual.
	struct OSKAPI_CALL PointLight {

		//Transform de la luz.
		alignas(16) glm::vec3 Position;

		//Color de la luz.
		alignas(16) Color Color;

		//Intensidad de la luz.
		alignas(16) float_t Intensity;

		//Radio de alcance de la luz.
		alignas(16) float_t Radius;

		//Variables de la luz.
		//Ecuaci�n constante.
		alignas(16) float_t Constant;

		//Variables de la luz.
		//Ecuaci�n lineal.
		alignas(16) float_t Linear;

		//Variables de la luz.
		//Ecuaci�n cuadr�tica.
		alignas(16) float_t Quadratic;

	};

}