#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "Log.h"

namespace OSK {

	//Representa un color en formato rgba.
	struct Color {

		//Crea una instancia vacía del color.
		Color();

		//Crea una instancia del color con los siguiente parámetros:
		//<r>: Red.
		//<g>: Green.
		//<b>: Blue.
		Color(const float_t& r, const float_t& g, const float_t& b);

		//Crea una instancia del color con los siguiente parámetros:
		//<r>: Red.
		//<g>: Green.
		//<b>: Blue.
		//<a>: Alpha.
		Color(const float_t& r, const float_t& g, const float_t& b, const float_t& a);

		//Crea una instancia del color con los siguiente parámetros:
		//<value>: valor para Red, Green y Blue.
		//Alpha = 1.0f.
		Color(const float_t& value);

		//Operación Color * float_t.
		//Sólo se modifica el Alpha.
		Color operator*(const float_t& value) const;
		
		//Cambia los valores del color, sin cambiar Alpha.
		void Update(float_t r, float_t g, float_t b);

		//Representa la intensidad del color rojo.
		float_t Red;

		//Representa la intensidad del color verde.
		float_t Green;

		//Representa la intensidad del color azul.
		float_t Blue;

		//Representa la opacidad del color.
		float_t Alpha;

	};

}
