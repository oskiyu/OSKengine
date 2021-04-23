#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "Log.h"

#include <glm.hpp>

namespace OSK {

	/// <summary>
	/// Representa un vector 3D.
	/// </summary>
	/// <typeparam name="T">PRecisión.</typeparam>
	template <typename T> class OSKAPI_CALL Vector3_t {

	public:

		/// <summary>
		/// Crea un vector 3D nulo { 0, 0, 0 }.
		/// </summary>
		Vector3_t() {
			X = 0;
			Y = 0;
			Z = 0;
		}

		/// <summary>
		/// Crea un vector 3D.
		/// </summary>
		Vector3_t(const T& x, const T& y, const T& z) {
			X = x;
			Y = y;
			Z = z;
		}

		/// <summary>
		/// Crea una instancia del Vector2 en la que X, Y y Z tienen en mismo valor.
		/// </summary>
		/// <param name="value">Valor para X, Y y Z.</param>
		Vector3_t(const T& value) {
			X = value;
			Y = value;
			Z = value;
		}

		/// <summary>
		/// Crea un vector 3D con los parámetros de vec.
		/// </summary>
		Vector3_t(const glm::vec3& vec) {
			X = vec.x;
			Y = vec.y;
			Z = vec.z;
		}

		/// <summary>
		/// Operación Vector3 + Vector3.
		/// X1 + X2; Y1 + Y2, Z1 + Z2.
		/// </summary>
		Vector3_t operator+(const Vector3_t& vec) const {
			return Vector3_t(X + vec.X, Y + vec.Y, Z + vec.Z);
		}

		/// <summary>
		/// Operación Vector3 += Vector3.
		/// X1 + X2; Y1 + Y2, Z1 + Z2.
		/// Modifica este vector.
		/// </summary>
		Vector3_t& operator+=(const Vector3_t& vec) {
			X += vec.X;
			Y += vec.Y;
			Z += vec.Z;
			return *this;
		}

		/// <summary>
		/// Negación del Vector3.
		/// -X; -Y; -Z.
		/// </summary>
		Vector3_t operator-() const {
			return Vector3_t(-X, -Y, -Z);
		}

		/// <summary>
		/// Operación Vector3 - Vector3.
		/// X1 - X2; Y1 - Y2; Z1 - Z2.
		/// </summary>
		Vector3_t operator-(const Vector3_t& vec) const {
			return Vector3_t(X - vec.X, Y - vec.Y, Z - vec.Z);
		}

		/// <summary>
		/// Operación Vector3 -= Vector3.
		/// X1 - X2; Y1 - Y2, Z1 - Z2.
		/// Modifica este vector.
		/// </summary>
		Vector3_t& operator-=(const Vector3_t& vec) {
			X -= vec.X;
			Y -= vec.Y;
			Z -= vec.Z;

			return *this;
		}

		/// <summary>
		/// Operación Vector3 * Vector3.
		/// X1 * X2; Y1 * Y2; Z1 * Z2.
		/// </summary>
		Vector3_t operator*(const Vector3_t& vec) const {
			return Vector3_t(X * vec.X, Y * vec.Y, Z * vec.Z);
		}

		/// <summary>
		/// Operación Vector3 *= Vector3.
		/// X1 * X2; Y1 * Y2, Z1 * Z2.
		/// Modifica este vector.
		/// </summary>
		Vector3_t& operator*=(const Vector3_t& vec) {
			X *= vec.X;
			Y *= vec.Y;
			Z *= vec.Z;

			return *this;
		}

		/// <summary>
		/// Operación Vector3 * T.
		/// X * value; Y * value; Z * value.
		/// </summary>
		Vector3_t operator*(const T& value) const {
			return Vector3_t(X * value, Y * value, Z * value);
		}

		//Operación Vector3 *= T.
		//X1 - X2; Y1 - Y2, Z1 - Z2.
		//Modifica este vector.

		/// <summary>
		/// Operación Vector3 *= T.
		/// X1 * T; Y1 * T, Z1 * T.
		/// Modifica este vector.
		/// </summary>
		Vector3_t& operator*=(const T& value) {
			X *= value;
			Y *= value;
			Z *= value;
			return *this;
		}

		/// <summary>
		/// Operación Vector3 / Vector3.
		/// X / vec.X; Y / vec.Y, Z / vec.Z.
		/// </summary>
		Vector3_t operator/(const Vector3_t& vec) const {
			return Vector3_t(X / vec.X, Y / vec.Y, Z / vec.Z);
		}

		/// <summary>
		/// Operación Vector3 /= Vector3.
		/// X1 / X2; Y1 / Y2, Z1 / Z2.
		/// Modifica este vector.
		/// </summary>
		Vector3_t& operator/=(const Vector3_t& vec) {
			X /= vec;
			Y /= vec;
			Z /= vec;

			return *this;
		}

		/// <summary>
		/// Operación Vector3 / float_t.
		/// X / value; Y / value; Z / value.
		/// </summary>
		Vector3_t operator/(const T& value) const {
			return Vector3_t(X / value, Y / value, Z / value);
		}

		/// <summary>
		/// Operación Vector3 /= T.
		/// X1 / value; Y1 / value, Z1 / Z2value
		/// Modifica este vector.
		/// </summary>
		Vector3_t& operator/=(const T& value) {
			X /= value;
			Y /= value;
			Z /= value;

			return *this;
		}

		/// <summary>
		/// Comparación.
		/// True si todos los componentes son iguales.
		/// </summary>
		bool operator==(const Vector3_t& vec) const {
			return X == vec.X && Y == vec.Y && Z == vec.Z;
		}

		/// <summary>
		/// Comparación.
		/// True si no todos los componentes son iguales.
		/// </summary>
		bool operator!=(const Vector3_t& vec) const {
			return !operator==(vec);
		}

		/// <summary>
		/// Módulo del vector.
		/// Obtiene la longitud del vector.
		/// </summary>
		T GetLenght() const {
			return glm::sqrt(X * X + Y * Y + Z * Z);
		}

		/// <summary>
		/// Obtiene la distancia entre la posición representada por este vector y por el vector 'vec'.
		/// </summary>
		T GetDistanceTo(const Vector3_t& vec) const {
			T dx = X - vec.X;
			T dy = Y - vec.Y;
			T dz = Z - vec.Z;

			return glm::sqrt(dx * dx + dy * dy + dz * dz);
		}

		/// <summary>
		/// Obtiene la distancia entre la posición representada por este vector y por el vector 'vec'.
		/// Al cuadrado.
		/// </summary>
		T GetDistanceTo2(const Vector3_t& vec) const {
			T dx = X - vec.X;
			T dy = Y - vec.Y;
			T dz = Z - vec.Z;

			return dx * dx + dy * dy + dz * dz;
		}

		/// <summary>
		/// Obtiene el producto escalar entre este vector y el vector 'vec'.
		/// </summary>
		T Dot(const Vector3_t& vec) const {
			return X * vec.X + Y * vec.Y + Z * vec.Z;
		}

		/// <summary>
		/// Obtiene el producto vectorial entre este vector y el vector 'vec'.
		/// </summary>
		Vector3_t Cross(const Vector3_t& vec) const {
			return Vector3_t(Y * vec.Z - Z * vec.Y, Z * vec.X - X * vec.Z, X * vec.Y - Y * vec.X);
		}

		/// <summary>
		/// Devuelve un vector 3D normalizado con la misma dirección que este.
		/// </summary>
		Vector3_t GetNormalized() const {
			T length = GetLenght();

			return Vector3_t(X / length, Y / length, Z / length);
		}

		/// <summary>
		/// Normaliza el vector 3D para que su módulo (GetLength()) sea 1.
		/// </summary>
		void Normalize() {
			T length = GetLenght();

			X /= length;
			Y /= length;
			Z /= length;
		}

		/// <summary>
		/// OSK::Vector2 a glm::vec3.
		/// </summary>
		glm::vec3 ToGLM() const {
			return glm::vec3(X, Y, Z);
		}

		/// <summary>
		/// OSK::Vector2 a glm::vec.
		/// </summary>
		inline glm::vec<3, T> ToGLM_T() const {
			return glm::vec<3, T>(X, Y, Z);
		}

		/// <summary>
		/// Devuelve un vector con variables de distinto tipo.
		/// P = nuevo tipo de las variables.
		/// </summary>
		template <typename P> inline Vector3_t<P> ToVec3() const {
			return Vector3_t<P>(X, Y, Z);
		}

		/// <summary>
		/// Devuelve un Vector3f a partir de este.
		/// </summary>
		inline Vector3_t<float_t> ToVector3f() const {
			return ToVec3<float>();
		}

		/// <summary>
		/// Devuelve un Vector3d a partir de este.
		/// </summary>
		inline Vector3_t<double_t> ToVector3d() const {
			return ToVec3<double_t>();
		}

		/// <summary>
		/// Devuelve un Vector3i a partir de este.
		/// </summary>
		inline Vector3_t<int32_t> ToVector3i() const {
			return ToVec3<int32_t>();
		}

		/// <summary>
		/// Devuelve un Vector3ui a partir de este.
		/// </summary>
		inline Vector3_t<uint32_t> ToVector3ui() const {
			return ToVec3<uint32_t>();
		}

		/// <summary>
		/// Representa la primera coordenada.
		/// </summary>
		T X;

		/// <summary>
		/// Representa la segunda coordenada.
		/// </summary>
		T Y;

		/// <summary>
		/// Representa la tercera coordenada.
		/// </summary>
		T Z;

	};

	/// <summary>
	/// Precisión = float.
	/// </summary>
	typedef Vector3_t<float_t> Vector3;
	
	/// <summary>
	/// Precisión = float.
	/// </summary>
	typedef Vector3_t<float_t> Vector3f;
	
	/// <summary>
	/// Precisión = double.
	/// </summary>
	typedef Vector3_t<double_t> Vector3d;
	
	/// <summary>
	/// Precisión = int32.
	/// </summary>
	typedef Vector3_t<int32_t> Vector3i;
	
	/// <summary>
	/// Precisión = uint32.
	/// </summary>
	typedef Vector3_t<uint32_t> Vector3ui;

}
