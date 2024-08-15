#pragma once

#include <glm/glm.hpp>

#include "ApiCall.h"

namespace OSK {

	/// @brief Representa un vector 2D.
	/// @tparam T Precisión.
	template <typename T> 
	class Vector2_t {

	public:

		/// @brief Tipo de dato numérico usado 
		/// para almacenar los números.
		using TNumericType = T;

		/// @brief 
		static OSKAPI_CALL Vector2_t const Zero;

		/// @brief 
		static OSKAPI_CALL Vector2_t const One;

	public:

		/// @brief Crea un vector 2D nulo { 0, 0 }.
		constexpr Vector2_t() : x(0), y(0) { }

		/// @brief Crea una instancia del Vector2.
		/// @param x Coordenada X.
		/// @param y Coordenada Y.
		constexpr Vector2_t(const T& x, const T& y) : x(x), y(y)  { }

		/// @brief Crea una instancia del Vector2 en la que X e Y tienen en mismo valor.
		/// @param value Coordenada X e Y.
		explicit constexpr Vector2_t(const T& value) : x(value), y(value) { }

		/// @brief Crea un vector 2D con los parámetros de vec.
		/// @param vec Vector.
		explicit constexpr Vector2_t(const glm::vec2& vec) : x(vec.x), y(vec.y) { }


		/// @return Operación Vector2 + Vector2.
		/// @param vec Otro vector.
		/// @note X1 + X2; Y1 + Y2.
		constexpr Vector2_t operator+(const Vector2_t& vec) const {
			return Vector2_t(x + vec.x, y + vec.y);
		}

		/// @return Operación Vector2 += Vector2.
		/// @note X1 + X2; Y1 + Y2.
		/// @note Modifica este vector.
		/// @param vec Otro vector.
		constexpr Vector2_t& operator+=(const Vector2_t& vec) {
			x += vec.x;
			y += vec.y;

			return *this;
		}
		
		/// @retrun Negación del Vector2.
		/// @note -X; -Y.
		constexpr Vector2_t operator-() const {
			return Vector2_t(-x, -y);
		}

		/// @return Operación Vector2 - Vector2.
		/// @note X1 - X2; Y1 - Y2.
		/// @param vec Otro vector.
		constexpr Vector2_t operator-(const Vector2_t& vec) const {
			return Vector2_t(x - vec.x, y - vec.y);
		}

		/// @return Operación Vector2 -= Vector2.
		/// @param vec Otro vector.
		/// @note X1 - X2; Y1 - Y2.
		/// @note Modifica este vector.
		constexpr Vector2_t& operator-=(const Vector2_t& vec) {
			x -= vec.x;
			y -= vec.y;

			return *this;
		}
		
		/// @return  Operación Vector2 * Vector2.
		/// @note X1 * X2; Y1 * Y2.
		/// @param vec Otro vector.
		constexpr Vector2_t operator*(const Vector2_t& vec) const {
			return Vector2_t(x * vec.x, y * vec.y);
		}

		/// @return Operación Vector2 *= Vector2.
		/// @param vec Otro vector.
		/// @note X1 * X2; Y1 * Y2.
		/// @note Modifica este vector.
		constexpr Vector2_t& operator*=(const Vector2_t& vec) {
			x *= vec.x;
			y *= vec.y;

			return *this;
		}
		
		/// @return Operación Vector2 * valor.
		/// @param value Valor por el que se va a multiplicar el vector.
		/// @note X * value; Y * value.
		constexpr Vector2_t operator*(const T& value) const {
			return Vector2_t(x * value, y * value);
		}

		/// @return Operación Vector2 * valor.
		/// @param value Valor por el que se va a multiplicar el vector.
		/// @note X * value; Y * value.
		/// @note Modifica este vector.
		constexpr Vector2_t& operator*=(const T& value) {
			x *= value;
			y *= value;

			return *this;
		}

		/// @return Operación Vector2 / valor.
		/// @param value Valor por el que se va a dividir el vector.
		/// @note X /= value; Y /= value.
		constexpr Vector2_t operator/(const T& value) const {
			return Vector2_t(x / value, y / value);
		}

		/// @return Operación Vector2 /= Vector2.
		/// @param value Valor por el que se va a dividir el vector.
		/// @note X = vec.X; Y = vec.Y.
		/// @note Modifica este vector.
		constexpr Vector2_t& operator/=(const Vector2_t& vec) {
			x /= vec.x;
			y /= vec.y;

			return *this;
		}

		/// @return Operación Vector2 / Vector2.
		/// @param value Otro vector por el que se va a dividir el vector.
		/// @note X = vec.X; Y = vec.Y.
		constexpr Vector2_t operator/(const Vector2_t& vec) const {
			return Vector2_t(x / vec.x, y / vec.y);
		}

		/// @return Operación Vector2 /= valor.
		/// @param value Valor por el que se va a dividir el vector.
		/// @note X = value; Y = value.
		/// @note Modifica este vector.
		constexpr Vector2_t& operator/=(const T& value) {
			x /= value;
			y /= value;

			return *this;
		}


		/// @return True si todos los componentes son iguales.
		/// @param vec Otro vector.
		constexpr bool operator==(const Vector2_t& vec) const {
			return x == vec.x && y == vec.y;
		}

		/// @param vec Otro vector.
		/// @return True si no todos los componentes son iguales.
		constexpr bool operator!=(const Vector2_t& vec) const {
			return !operator==(vec);
		}


		/// @return Módulo del vector.
		/// Obtiene la longitud del vector.
		constexpr T GetLength() const {
			return glm::sqrt(x * x + y * y);
		}
				
		/// @return Obtiene la distancia entre la posición representada por este vector y por el vector 'vec'.
		/// @param vec Otro vector.
		constexpr T GetDistanceTo(const Vector2_t& vec) const {
			T dx = x - vec.x;
			T dy = y - vec.y;

			return glm::sqrt(dx * dx + dy * dy);
		}

		/// @brief Producto escalar de dos vectores.
		/// @param vec Otro vector.
		constexpr T Dot(const Vector2_t& vec) const {
			return x * vec.x + y * vec.y;
		}


		/// @return Devuelve un vector 2D normalizado con la misma dirección que este.
		constexpr Vector2_t GetNormalized() const {
			T length = GetLength();

			return Vector2_t(x / length, y / length);
		}

		/// @return Normaliza este vector 2D para que su módulo (GetLength()) sea 1.
		constexpr void Normalize() {
			T length = GetLength();

			x /= length;
			y /= length;
		}


		/// @return OSK::Vector2 a glm::vec2.
		constexpr glm::vec2 ToGlm() const {
			return glm::vec2(x, y);
		}

		/// @return OSK::Vector2 a glm::vec.
		constexpr glm::vec<2, T> ToGlm_t() const {
			return glm::vec<2, T>(x, y);
		}


		/// @return Devuelve un vector con variables de distinto tipo.
		/// P = nuevo tipo de las variables.
		template <typename P> 
		constexpr Vector2_t<P> ToVec2() const {
			return Vector2_t<P>((P)x, (P)y);
		}

		/// @return Vector 2 con precisión float.
		constexpr Vector2_t<float_t> ToVector2f() const {
			return ToVec2<float>();
		}

		/// @return Vector 2 con precisión double.
		constexpr Vector2_t<double_t> ToVector2d() const {
			return ToVec2<double_t>();
		}

		/// @return Vector 2 con precisión int32.
		constexpr Vector2_t<int32_t> ToVector2i() const {
			return ToVec2<int32_t>();
		}

		/// @return Vector 2 con precisión uint32.
		constexpr Vector2_t<uint32_t> ToVector2ui() const {
			return ToVec2<uint32_t>();
		}


		/// @brief Representa la primera coordenada.
		T x;

		/// @brief Representa la segunda coordenada.
		T y;

	};


	/// @brief Precisión = float.
	using Vector2f = Vector2_t<float_t>;

	/// @brief Precisión = double.
	using Vector2d = Vector2_t<double_t>;

	/// @brief Precisión = int32.
	using Vector2i = Vector2_t<int32_t>;

	/// @brief Precisión = uint32.
	using Vector2ui = Vector2_t<uint32_t>;

}
