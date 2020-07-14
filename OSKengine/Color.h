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
		
		//Devuelve un glm::vec4 con los valores de este color.
		glm::vec4 ToGLM() const;

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

		static inline Color BLACK() {
			return Color(0.0f, 0.0f, 0.0f);
		}

		static inline Color WHITE() {
			return Color(1.0f);
		}

		static inline Color BLUE() {
			return Color(0.0f, 1.0f, 1.0f);
		}

		static inline Color PURPLE() {
			return Color(1.0f, 0.0f, 1.0f);
		}

		static inline Color RED() {
			return Color(1.0f, 0.0f, 0.0f);
		}

		static inline Color YELLOW() {
			return Color(1.0f, 1.0f, 0.0f);
		}

	};

}
