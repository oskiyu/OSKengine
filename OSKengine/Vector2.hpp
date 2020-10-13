#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "Log.h"

#include <glm.hpp>

namespace OSK {

	//Representa un vector 2D.
	template <typename T> class Vector2_t {

	public:

		//Crea un vector 2D nulo { 0, 0 }.
		Vector2_t() {
			X = 0;
			Y = 0;
		}

		//Crea una instancia del Vector2.
		Vector2_t(const T& x, const T& y) {
			X = x;
			Y = y;
		}

		//Crea una instancia del Vector2 en la que X e Y tienen en mismo valor.
		Vector2_t(const T& value) {
			X = value;
			Y = value;
		}

		//Crea un vector 2D con los parámetros de vec.
		Vector2_t(const glm::vec2& vec) {
			X = vec.x;
			Y = vec.y;
		}


		//Operación Vector2 + Vector2.
		//X1 + X2; Y1 + Y2.
		inline Vector2_t operator+(const Vector2_t& vec) const {
			return Vector2_t(X + vec.X, Y + vec.Y);
		}

		//Operación Vector2 += Vector2.
		//X1 + X2; Y1 + Y2.
		//Modifica este vector.
		inline Vector2_t& operator+=(const Vector2_t& vec) {
			X += vec.X;
			Y += vec.Y;

			return *this;
		}
		
		//Negación del Vector2.
		//-X; -Y.
		inline Vector2_t operator-() const {
			return Vector2_t(-X, -Y);
		}
		
		//Operación Vector2 - Vector2.
		//X1 - X2; Y1 - Y2.
		inline Vector2_t operator-(const Vector2_t& vec) const {
			return Vector2_t(X - vec.X, Y - vec.Y);
		}

		//Operación Vector2 -= Vector2.
		//X1 - X2; Y1 - Y2.
		//Modifica este vector.
		inline Vector2_t& operator-=(const Vector2_t& vec) {
			X -= vec.X;
			Y -= vec.Y;

			return *this;
		}
		
		//Operación Vector2 * Vector2.
		//X1 * X2; Y1 * Y2.
		inline Vector2_t operator*(const Vector2_t& vec) const {
			return Vector2_t(X * vec.X, Y * vec.Y);
		}

		//Operación Vector2 *= Vector2.
		//X1 * X2; Y1 * Y2.
		//Modifica este vector.
		inline Vector2_t& operator*=(const Vector2_t& vec) {
			X *= vec.X;
			Y *= vec.Y;

			return *this;
		}
		
		//Operación Vector2 * float_t.
		//X * value; Y * value.
		inline Vector2_t operator*(const T& value) const {
			return Vector2_t(X * value, Y * value);
		}

		//Operación Vector2 * float_t.
		//X * value; Y * value.
		//Modifica este vector.
		inline Vector2_t& operator*=(const T& value) {
			X *= value;
			Y *= value;

			return *this;
		}

		//Operación Vector2 / float_t.
		//X = value; Y = value.
		inline Vector2_t operator/(const T& value) const {
			return Vector2_t(X / value, Y / value);
		}

		//Operación Vector2 /= Vector2.
		//X = vec.X; Y = vec.Y.
		//Modifica este vector.
		inline Vector2_t& operator/=(const Vector2_t& vec) {
			X /= vec.X;
			Y /= vec.Y;

			return *this;
		}

		//Operación Vector2 / Vector2.
		//X = vec.X; Y = vec.Y.
		inline Vector2_t operator/(const Vector2_t& vec) const {
			return Vector2(X / vec.X, Y / vec.Y);
		}

		//Operación Vector2 /= float_t.
		//X = value; Y = value.
		//Modifica este vector.
		inline Vector2_t& operator/=(const T& value) {
			X /= value;
			Y /= value;

			return *this;
		}

		//Comparación.
		//True si todos los componentes son iguales.
		bool operator==(const Vector2_t& vec) const {
			return X == vec.X && Y == vec.Y;
		}

		//Comparación.
		//True si no todos los componentes son iguales.
		bool operator!=(const Vector2_t& vec) const {
			return !operator==(vec);
		}

		//Módulo del vector.
		//Obtiene la longitud del vector.
		inline T GetLength() const {
			return glm::sqrt(X * X + Y * Y);
		}

		//Obtiene la distancia entre la posición representada por este vector y por el vector <vec>.
		inline T GetDistanceTo(const Vector2_t& vec) const {
			T dx = X - vec.X;
			T dy = Y - vec.Y;

			return glm::sqrt(dx * dx + dy * dy);
		}

		//Producto escalar de dos vectores.
		inline T Dot(const Vector2_t& vec) const {
			return X * vec.X + Y * vec.Y;
		}

		//Devuelve un vector 2D normalizado con la misma dirección que este.
		Vector2_t GetNormalized() const {
			T length = GetLength();

			return Vector2_t(X / length, Y / length);
		}

		//Normaliza este vector 2D para que su módulo (GetLength()) sea 1.
		void Normalize() {
			T length = GetLength();

			X /= length;
			Y /= length;
		}

		//OSK::Vector2 a glm::vec2.
		inline glm::vec2 ToGLM() const {
			return glm::vec2(X, Y);
		}

		//OSK::Vector2 a glm::vec.
		inline glm::vec<2, T> ToGLM_T() const {
			return glm::vec<2, T>(X, Y);
		}

		//Devuelve un vector con variables de distinto tipo.
		//P = nuevo tipo de las variables.
		template <typename P> inline Vector2_t<P> ToVec2() const {
			return Vector2_t<P>(X, Y);
		}

		//Devuelve un Vector2f a partir de este.
		inline Vector2_t<float_t> ToVector2f() const {
			return ToVec2<float>();
		}

		//Devuelve un Vector2d a partir de este.
		inline Vector2_t<double_t> ToVector2d() const {
			return ToVec2<double_t>();
		}

		//Devuelve un Vector2i a partir de este.
		inline Vector2_t<int32_t> ToVector2i() const {
			return ToVec2<int32_t>();
		}

		//Devuelve un Vector2ui a partir de este.
		inline Vector2_t<uint32_t> ToVector2ui() const {
			return ToVec2<uint32_t>();
		}

		//Representa la primera coordenada.
		T X;

		//Representa la segunda coordenada.
		T Y;

	};


	//Representa un vector 2D.
	//Precisión = float.
	typedef Vector2_t<float_t> Vector2;

	
	//Representa un vector 2D.
	//Precisión = float.
	typedef Vector2_t<float_t> Vector2f;

	//Representa un vector 2D.
	//Precisión = double.
	typedef Vector2_t<double_t> Vector2d;

	//Representa un vector 2D.
	//Precisión = int32.
	typedef Vector2_t<int32_t> Vector2i;

	//Representa un vector 2D.
	//Precisión = uint32.
	typedef Vector2_t<uint32_t> Vector2ui;

}
