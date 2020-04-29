#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Color.h"
#include "Transform.h"

namespace OSK {
	
	//Representa una luz puntual.
	struct PointLight {

		//Transform de la luz.
		Transform LightTransform;

		//Color de la luz.
		Color Color;

		//Intensidad de la luz.
		float_t Intensity;

		//Radio de alcance de la luz.
		float_t Radius;

		//Variables de la luz.
		//Ecuaci�n constante.
		float_t Constant;

		//Variables de la luz.
		//Ecuaci�n lineal.
		float_t Linear;

		//Variables de la luz.
		//Ecuaci�n cuadr�tica.
		float_t Quadratic;

	};

}