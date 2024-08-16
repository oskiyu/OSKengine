#pragma once

#include "ApiCall.h"
#include "Serializer.h"

#include <glm/glm.hpp>


namespace OSK {

	/// @brief Representa un color en formato RGBA float.
	class OSKAPI_CALL Color {

	public:

		OSK_SERIALIZABLE();

		/// @brief Color vacío (negro y transparente).
		constexpr Color() = default;


		/// @brief Crea una instancia del color con los siguiente parámetros:
		/// @param r Intensidad del color rojo (rango 0-255).
		/// @param g Intensidad del color verde (rango 0-255).
		/// @param b Intensidad del color azul (rango 0-255).
		constexpr static Color FromBytes(TByte r, TByte g, TByte b) {
			return FromBytes(r, g, b, 255);
		}

		/// @brief Crea una instancia del color con los siguiente parámetros:
		/// @param r Intensidad del color rojo (rango 0-255).
		/// @param g Intensidad del color verde (rango 0-255).
		/// @param b Intensidad del color azul (rango 0-255).
		/// @param a Opacidad (rango 0-255).
		constexpr static Color FromBytes(TByte r, TByte g, TByte b, TByte a) {
			return Color(
				static_cast<float>(r) / 255.0f,
				static_cast<float>(g) / 255.0f,
				static_cast<float>(b) / 255.0f,
				static_cast<float>(a) / 255.0f);
		}

		/// @brief Crea una instancia del color con los siguiente parámetros:
		/// @param r Intensidad del color rojo (rango 0.0-1.0).
		/// @param g Intensidad del color verde (rango 0.0-1.0).
		/// @param b Intensidad del color azul (rango 0.0-1.0).
		constexpr Color(float r, float g, float b) :
			red(r),
			green(g),
			blue(b),
			alpha(1.0f) { }

		/// @brief Crea una instancia del color con los siguiente parámetros:
		/// @param r Intensidad del color rojo (rango 0.0-1.0).
		/// @param g Intensidad del color verde (rango 0.0-1.0).
		/// @param b Intensidad del color azul (rango 0.0-1.0).
		/// @param a Opacidad (rango 0.0-1.0).
		constexpr Color(float r, float g, float b, float a) :
			red(r),
			green(g),
			blue(b),
			alpha(a) { }

		/// @brief Crea una instancia del color.
		/// @param value Intensidad del rojo verde y azul (rango 0.0-1.0).
		/// @note Alpha = 1.0f.
		constexpr explicit Color(float value) :
			red(value),
			green(value),
			blue(value),
			alpha(1.0f) { }

		/// @brief Multiplica el valor de opacidad por el valor dado.
		/// @param value Valor multiplicador..
		/// @return Color(red, green, blue, alpha * @p value).
		constexpr Color operator*(float value) const {
			return Color(red, green, blue, alpha * value);
		}

		/// @brief Suma aditiva del color.
		/// @param other Otro color.
		/// @return Color + Color.
		constexpr Color operator+(const Color& other) const {
			return Color(
				red + other.red * other.alpha,
				green + other.green * other.alpha,
				blue + other.blue * other.alpha,
				alpha + other.alpha);
		}

		/// @brief Suma aditiva del color.
		/// @param other Otro color.
		constexpr void operator+=(const Color& other) {
			*this = *this + other;
		}

		/// @return Devuelve un glm::vec4 con los valores de este color.
		constexpr glm::vec4 ToGlm() const {
			return glm::vec4(red, green, blue, alpha);
		}


		/// @brief Representa la intensidad del color rojo.
		float red = 0.0f;

		/// @brief Representa la intensidad del color verde.
		float green = 0.0f;

		/// @brief Representa la intensidad del color azul.
		float blue = 0.0f;

		/// @brief Representa la opacidad del color.
		float alpha = 0.0f;


		/// @brief Color vacío (0, 0, 0, 0).
		static Color const Empty;
		/// @brief Color negro (0, 0, 0, 1).
		static Color const Black;
		/// @brief Color blanco (1, 1, 1, 1).
		static Color const White;
		/// @brief Color azul (0, 0, 1, 1).
		static Color const Blue;
		/// @brief Color morado (1, 0, 1, 1).
		static Color const Purple;
		/// @brief Color rojo (1, 0, 0, 1).
		static Color const Red;
		/// @brief Color amarillo (1, 1, 0, 1).
		static Color const Yellow;
		/// @brief Color verde (0, 1, 0, 1).
		static Color const Green;

	};

}

OSK_SERIALIZATION(OSK::Color);
