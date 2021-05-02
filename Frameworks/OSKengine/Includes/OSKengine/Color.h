#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "Log.h"

#include <glm.hpp>

namespace OSK {

	/// <summary>
	/// Representa un color en formato RGBA.
	/// </summary>
	struct OSKAPI_CALL Color {

		/// <summary>
		/// Crea una instancia vacía del color.
		/// </summary>
		Color();

		/// <summary>
		/// Crea una instancia del color con los siguiente parámetros:
		/// </summary>
		/// <param name="r">Red.</param>
		/// <param name="g">Green.</param>
		/// <param name="b">Blue.</param>
		Color(float r, float g, float b);

		/// <summary>
		/// Crea una instancia del color con los siguiente parámetros:
		/// </summary>
		/// <param name="r">Red.</param>
		/// <param name="g">Green.</param>
		/// <param name="b">Blue.</param>
		/// <param name="a">Alpha.</param>
		Color(float r, float g, float b, float a);

		/// <summary>
		/// Crea una instancia del color con los siguiente parámetros:
		/// Alpha = 1.0f.
		/// </summary>
		/// <param name="value">valor para Red, Green y Blue.</param>
		Color(float value);

		/// <summary>
		/// Operación Color * float.
		/// Sólo se modifica el Alpha.
		/// </summary>
		/// <param name="value">Nuevo valor de alfa.</param>
		/// <returns>Color.</returns>
		Color operator*(float value) const;
		
		/// <summary>
		/// Devuelve un glm::vec4 con los valores de este color.
		/// </summary>
		/// <returns>Devuelve el color en GLM.</returns>
		glm::vec4 ToGLM() const;

		/// <summary>
		/// Cambia los valores del color, sin cambiar Alpha.
		/// </summary>
		/// <param name="r">Red.</param>
		/// <param name="g">Green.</param>
		/// <param name="b">Blue.</param>
		void Update(float r, float g, float b);

		/// <summary>
		/// Representa la intensidad del color rojo.
		/// </summary>
		float Red;

		/// <summary>
		/// Representa la intensidad del color verde.
		/// </summary>
		float Green;

		/// <summary>
		/// Representa la intensidad del color azul.
		/// </summary>
		float Blue;

		/// <summary>
		/// Representa la opacidad del color.
		/// </summary>
		float Alpha;

		/// <summary>
		/// Color negro. (0.0f, 0.0f, 0.0f)
		/// </summary>
		/// <returns>Color negro.</returns>
		static inline Color BLACK() {
			return Color(0.0f, 0.0f, 0.0f);
		}

		/// <summary>
		/// Color blanco. (1.0f).
		/// </summary>
		/// <returns>Color blanco.</returns>
		static inline Color WHITE() {
			return Color(1.0f);
		}

		/// <summary>
		/// Color azul. (0.0f, 1.0f, 1.0f).
		/// </summary>
		/// <returns>Color azul.</returns>
		static inline Color BLUE() {
			return Color(0.0f, 1.0f, 1.0f);
		}

		/// <summary>
		/// Color morado. (1.0f, 0.0f, 1.0f).
		/// </summary>
		/// <returns>Color morado.</returns>
		static inline Color PURPLE() {
			return Color(1.0f, 0.0f, 1.0f);
		}

		/// <summary>
		/// Color rojo. (1.0f, 0.0f, 0.0f).
		/// </summary>
		/// <returns>Color rojo.</returns>
		static inline Color RED() {
			return Color(1.0f, 0.0f, 0.0f);
		}

		/// <summary>
		/// Color amarillo. (1.0f, 1.0f, 0.0f).
		/// </summary>
		/// <returns>Color amarillo.</returns>
		static inline Color YELLOW() {
			return Color(1.0f, 1.0f, 0.0f);
		}

	};

}
