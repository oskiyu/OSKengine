#pragma once

#include "Wrapper.h"
#include <OSKengine/Vector3.hpp>

namespace OSKengine {

	template <typename T> public ref class Vector3_t : Wrapper<OSK::Vector3_t<T>> {

	public:

		Vector3_t() : Wrapper(new OSK::Vector3_t<T>()) {

		}
		Vector3_t(T x, T y, T z) : Wrapper(new OSK::Vector3_t<T>(x, y, z)) {
			X = x;
			Y = y;
			Z = z;
		}
		Vector3_t(T value) : Wrapper(new OSK::Vector3_t<T>(value) {
			X = value;
			Y = value;
			Z = value;
		}

		Vector3_t operator+(Vector3_t^ vec) {
			return Vector3_t(X + vec.X, Y + vec.Y, Z + vec.Z);
		}
		Vector3_t% operator+=(Vector3_t^ vec) {
			X += vec.X;
			Y += vec.Y;
			Z += vec.Z;
			return *this;
		}

		Vector3_t operator-() const {
			return Vector3_t(-X, -Y, -Z);
		}
		Vector3_t operator-(const Vector3_t& vec) const {
			return Vector3_t(X - vec.X, Y - vec.Y, Z - vec.Z);
		}

		//Operación Vector3 -= Vector3.
		//X1 - X2; Y1 - Y2, Z1 - Z2.
		//Modifica este vector.
		Vector3_t& operator-=(const Vector3_t& vec) {
			X -= vec.X;
			Y -= vec.Y;
			Z -= vec.Z;

			return *this;
		}

		//Operación Vector3 * Vector3.
		//X1 * X2; Y1 * Y2; Z1 * Z2.
		Vector3_t operator*(const Vector3_t& vec) const {
			return Vector3_t(X * vec.X, Y * vec.Y, Z * vec.Z);
		}

		//Operación Vector3 *= Vector3.
		//X1 * X2; Y1 * Y2, Z1 * Z2.
		//Modifica este vector.
		Vector3_t& operator*=(const Vector3_t& vec) {
			X *= vec.X;
			Y *= vec.Y;
			Z *= vec.Z;

			return *this;
		}

		//Operación Vector3 * T.
		//X * value; Y * value; Z * value.
		Vector3_t operator*(const T& value) const {
			return Vector3_t(X * value, Y * value, Z * value);
		}

		//Operación Vector3 *= T.
		//X1 - X2; Y1 - Y2, Z1 - Z2.
		//Modifica este vector.
		Vector3_t& operator*=(const T& value) {
			X *= value;
			Y *= value;
			Z *= value;
			return *this;
		}

		//Operación Vector3 / Vector3.
		//X / vec.X; Y / vec.Y, Z / vec.Z.
		Vector3_t operator/(const Vector3_t& vec) const {
			return Vector3_t(X / vec.X, Y / vec.Y, Z / vec.Z);
		}

		//Operación Vector3 /= Vector3.
		//X1 / X2; Y1 / Y2, Z1 / Z2.
		//Modifica este vector.
		Vector3_t& operator/=(const Vector3_t& vec) {
			X /= vec;
			Y /= vec;
			Z /= vec;

			return *this;
		}

		//Operación Vector3 / float_t.
		//X / value; Y / value; Z / value.
		Vector3_t operator/(const T& value) const {
			return Vector3_t(X / value, Y / value, Z / value);
		}

		//Operación Vector3 /= T.
		//X1 / value; Y1 / value, Z1 / Z2value
		//Modifica este vector.
		Vector3_t& operator/=(const T& value) {
			X /= value;
			Y /= value;
			Z /= value;

			return *this;
		}

		//Comparación.
		//True si todos los componentes son iguales.
		bool operator==(const Vector3_t& vec) const {
			return X == vec.X && Y == vec.Y && Z == vec.Z;
		}

		//Comparación.
		//True si no todos los componentes son iguales.
		bool operator!=(const Vector3_t& vec) const {
			return !operator==(vec);
		}

		//Módulo del vector.
		//Obtiene la longitud del vector.
		T GetLenght() const {
			return glm::sqrt(X * X + Y * Y + Z * Z);
		}

		//Obtiene la distancia entre la posición representada por este vector y por el vector <vec>.
		T GetDistanceTo(const Vector3_t& vec) const {
			T dx = X - vec.X;
			T dy = Y - vec.Y;
			T dz = Z - vec.Z;

			return glm::sqrt(dx * dx + dy * dy + dz * dz);
		}

		//Obtiene la distancia entre la posición representada por este vector y por el vector <vec>.
		//Al cuadrado.
		T GetDistanceTo2(const Vector3_t& vec) const {
			T dx = X - vec.X;
			T dy = Y - vec.Y;
			T dz = Z - vec.Z;

			return dx * dx + dy * dy + dz * dz;
		}

		//Obtiene el producto escalar entre este vector y el vector <vec>.
		T Dot(const Vector3_t& vec) const {
			return X * vec.X + Y * vec.Y + Z * vec.Z;
		}

		//Obtiene el producto vectorial entre este vector y el vector <vec>.
		Vector3_t Cross(const Vector3_t& vec) const {
			return Vector3_t(Y * vec.Z - Z * vec.Y, Z * vec.X - X * vec.Z, X * vec.Y - Y * vec.X);
		}

		//Devuelve un vector 3D normalizado con la misma dirección que este.
		Vector3_t GetNormalized() const {
			T length = GetLenght();

			return Vector3_t(X / length, Y / length, Z / length);
		}

		//Normaliza el vector 3D para que su módulo (GetLength()) sea 1.
		void Normalize() {
			T length = GetLenght();

			X /= length;
			Y /= length;
			Z /= length;
		}

		//OSK::Vector2 a glm::vec3.
		glm::vec3 ToGLM() const {
			return glm::vec3(X, Y, Z);
		}

		//OSK::Vector2 a glm::vec.
		inline glm::vec<3, T> ToGLM_T() const {
			return glm::vec<3, T>(X, Y, Z);
		}

		//Devuelve un vector con variables de distinto tipo.
		//P = nuevo tipo de las variables.
		template <typename P> inline Vector3_t<P> ToVec3() const {
			return Vector3_t<P>(X, Y, Z);
		}

		//Devuelve un Vector3f a partir de este.
		inline Vector3_t<float_t> ToVector3f() const {
			return ToVec3<float>();
		}

		//Devuelve un Vector3d a partir de este.
		inline Vector3_t<double_t> ToVector3d() const {
			return ToVec3<double_t>();
		}

		//Devuelve un Vector3i a partir de este.
		inline Vector3_t<int32_t> ToVector3i() const {
			return ToVec3<int32_t>();
		}

		//Devuelve un Vector3ui a partir de este.
		inline Vector3_t<uint32_t> ToVector3ui() const {
			return ToVec3<uint32_t>();
		}

		//Representa la primera coordenada.
		T X;

		//Representa la segunda coordenada.
		T Y;

		//Representa la tercera coordenada.
		T Z;

	};

}