#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "Log.h"

#include <glm.hpp>

namespace OSK {

	//Representa un color en formato rgba.
	struct OSKAPI_CALL Color {

		//Crea una instancia vacía del color.
		Color();

		//Crea una instancia del color con los siguiente parámetros:
		//<r>: Red.
		//<g>: Green.
		//<b>: Blue.
		Color(float r, float g, float b);

		//Crea una instancia del color con los siguiente parámetros:
		//<r>: Red.
		//<g>: Green.
		//<b>: Blue.
		//<a>: Alpha.
		Color(float r, float g, float b, float a);

		//Crea una instancia del color con los siguiente parámetros:
		//<value>: valor para Red, Green y Blue.
		//Alpha = 1.0f.
		Color(float value);

		//Operación Color * float_t.
		//Sólo se modifica el Alpha.
		Color operator*(float value) const;
		
		//Devuelve un glm::vec4 con los valores de este color.
		glm::vec4 ToGLM() const;

		//Cambia los valores del color, sin cambiar Alpha.
		void Update(float r, float g, float b);

		//Representa la intensidad del color rojo.
		float Red;

		//Representa la intensidad del color verde.
		float Green;

		//Representa la intensidad del color azul.
		float Blue;

		//Representa la opacidad del color.
		float Alpha;

		//Colores por defecto:

		//Color negro.
		static inline Color BLACK() {
			return Color(0.0f, 0.0f, 0.0f);
		}

		//Color blanco.
		static inline Color WHITE() {
			return Color(1.0f);
		}

		//Color azul.
		static inline Color BLUE() {
			return Color(0.0f, 1.0f, 1.0f);
		}

		//Color morado.
		static inline Color PURPLE() {
			return Color(1.0f, 0.0f, 1.0f);
		}

		//Color rojo.
		static inline Color RED() {
			return Color(1.0f, 0.0f, 0.0f);
		}

		//Color amarillo.
		static inline Color YELLOW() {
			return Color(1.0f, 1.0f, 0.0f);
		}

	};

}
