#pragma once

#include <glm/glm.hpp>

#include "ToString.h"

namespace OSK {

	/// <summary>
	/// Representa un vector 3D.
	/// </summary>
	/// <typeparam name="T">PRecisión.</typeparam>
	template <typename T> class Vector3_t {

	public:

		static Vector3_t const Zero;

	public:

		/// <summary>
		/// Crea un vector 3D nulo { 0, 0, 0 }.
		/// </summary>
		Vector3_t() {
			this->x = 0;
			this->y = 0;
			this->Z = 0;
		}

		/// <summary>
		/// Crea un vector 3D.
		/// </summary>
		Vector3_t(const T& x, const T& y, const T& z) {
			this->x = x;
			this->y = y;
			this->Z = z;
		}

		/// <summary>
		/// Crea una instancia del Vector2 en la que X, Y y Z tienen en mismo valor.
		/// </summary>
		/// <param name="value">Valor para X, Y y Z.</param>
		Vector3_t(const T& value) {
			this->x = value;
			this->y = value;
			this->Z = value;
		}

		/// <summary>
		/// Crea un vector 3D con los parámetros de vec.
		/// </summary>
		Vector3_t(const glm::vec3& vec) {
			this->x = vec.x;
			this->y = vec.y;
			this->Z = vec.z;
		}

		/// <summary>
		/// Operación Vector3 + Vector3.
		/// 
		/// @note X1 + X2; Y1 + Y2, Z1 + Z2.
		/// </summary>
		Vector3_t operator+(const Vector3_t& vec) const {
			return Vector3_t(x + vec.x, y + vec.y, Z + vec.Z);
		}

		/// <summary>
		/// Operación Vector3 += Vector3.
		/// 
		/// @note X1 + X2; Y1 + Y2, Z1 + Z2.
		/// @note Modifica este vector.
		/// </summary>
		Vector3_t& operator+=(const Vector3_t& vec) {
			x += vec.x;
			y += vec.y;
			Z += vec.Z;
			return *this;
		}

		/// <summary>
		/// Negación del Vector3.
		/// 
		/// @note -X; -Y; -Z.
		/// </summary>
		Vector3_t operator-() const {
			return Vector3_t(-x, -y, -Z);
		}

		/// <summary>
		/// Operación Vector3 - Vector3.
		/// 
		/// @note X1 - X2; Y1 - Y2; Z1 - Z2.
		/// </summary>
		Vector3_t operator-(const Vector3_t& vec) const {
			return Vector3_t(x - vec.x, y - vec.y, Z - vec.Z);
		}

		/// <summary>
		/// Operación Vector3 -= Vector3.
		/// 
		/// @note X1 - X2; Y1 - Y2, Z1 - Z2.
		/// @note Modifica este vector.
		/// </summary>
		Vector3_t& operator-=(const Vector3_t& vec) {
			x -= vec.x;
			y -= vec.y;
			Z -= vec.Z;

			return *this;
		}

		/// <summary>
		/// Operación Vector3 * Vector3.
		/// 
		/// @note X1 * X2; Y1 * Y2; Z1 * Z2.
		/// </summary>
		Vector3_t operator*(const Vector3_t& vec) const {
			return Vector3_t(x * vec.x, y * vec.y, Z * vec.Z);
		}

		/// <summary>
		/// Operación Vector3 *= Vector3.
		/// 
		/// @note X1 * X2; Y1 * Y2, Z1 * Z2.
		/// @note Modifica este vector.
		/// </summary>
		Vector3_t& operator*=(const Vector3_t& vec) {
			x *= vec.x;
			y *= vec.y;
			Z *= vec.Z;

			return *this;
		}

		/// <summary>
		/// Operación Vector3 * T.
		/// 
		/// @note X * value; Y * value; Z * value.
		/// </summary>
		Vector3_t operator*(const T& value) const {
			return Vector3_t(x * value, y * value, Z * value);
		}

		/// <summary>
		/// Operación Vector3 *= T.
		/// 
		/// @note X1 * T; Y1 * T, Z1 * T.
		/// @note Modifica este vector.
		/// </summary>
		Vector3_t& operator*=(const T& value) {
			x *= value;
			y *= value;
			Z *= value;
			return *this;
		}

		/// <summary>
		/// Operación Vector3 / Vector3.
		/// 
		/// @note X / vec.X; Y / vec.Y, Z / vec.Z.
		/// </summary>
		Vector3_t operator/(const Vector3_t& vec) const {
			return Vector3_t(x / vec.x, y / vec.y, Z / vec.Z);
		}

		/// <summary>
		/// Operación Vector3 /= Vector3.
		/// 
		/// @note X1 / X2; Y1 / Y2, Z1 / Z2.
		/// @note Modifica este vector.
		/// </summary>
		Vector3_t& operator/=(const Vector3_t& vec) {
			x /= vec;
			y /= vec;
			Z /= vec;

			return *this;
		}

		/// <summary>
		/// Operación Vector3 / float_t.
		/// 
		/// @note X / value; Y / value; Z / value.
		/// </summary>
		Vector3_t operator/(const T& value) const {
			return Vector3_t(x / value, y / value, Z / value);
		}

		/// <summary>
		/// Operación Vector3 /= T.
		/// 
		/// @note X1 / value; Y1 / value, Z1 / Z2value
		/// @note Modifica este vector.
		/// </summary>
		Vector3_t& operator/=(const T& value) {
			x /= value;
			y /= value;
			Z /= value;

			return *this;
		}

		/// @brief Comprueba si dos vectores son iguales, de
		/// acuerdo a un épsilon para manejar casos de error
		/// de redondeo.
		/// @param other Otro vector. 
		/// @param epsilon Distancia mínima en cada eje que se considera error
		/// de redondeo, por lo que dos puntos a una distancia menor se consideran
		/// idénticos.
		/// @return True si son iguales, false en caso contrario.
		constexpr bool Equals(const Vector3_t& other, float epsilon) const {
			return
				glm::abs(x - other.x) < epsilon &&
				glm::abs(y - other.y) < epsilon &&
				glm::abs(Z - other.Z) < epsilon;
		}

		/// <summary>
		/// Comparación.
		/// True si todos los componentes son iguales.
		/// </summary>
		bool operator==(const Vector3_t& vec) const {
			return x == vec.x && y == vec.y && Z == vec.Z;
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
			return glm::sqrt(x * x + y * y + Z * Z);
		}

		/// <summary>
		/// Obtiene la distancia entre la posición representada por este vector y por el vector 'vec'.
		/// </summary>
		T GetDistanceTo(const Vector3_t& vec) const {
			const T dx = x - vec.x;
			const T dy = y - vec.y;
			const T dz = Z - vec.Z;

			return glm::sqrt(dx * dx + dy * dy + dz * dz);
		}

		/// <summary>
		/// Obtiene la distancia entre la posición representada por este vector y por el vector 'vec'.
		/// Al cuadrado.
		/// </summary>
		T GetDistanceTo2(const Vector3_t& vec) const {
			T dx = x - vec.x;
			T dy = y - vec.y;
			T dz = Z - vec.Z;

			return dx * dx + dy * dy + dz * dz;
		}

		/// <summary>
		/// Obtiene el producto escalar entre este vector y el vector 'vec'.
		/// </summary>
		T Dot(const Vector3_t& vec) const {
			// return glm::dot(this->ToGLM(), vec.ToGLM());
			return x * vec.x + y * vec.y + Z * vec.Z;
		}

		/// <summary>
		/// Devuelve el ángulo, en grados, entre este vector y el vector dado.
		/// 
		/// El tipo de ángulo dependerá del resultado de this->Dot(vec):
		/// - Agudo: resultado positivo.
		/// - Recto: resultado 0.
		/// - Obtuso: resultado negativo.
		/// </summary>
		T GetAngle(const Vector3_t& vec) const {
			return glm::degrees(glm::acos(this->Dot(vec) / (this->GetLenght() * vec.GetLenght())));
		}

		//Vector3_t ProjectTo(const Vector3_t& vec) const {
		//	return this->Dot(vec) / (vec.GetLenght() * vec.GetLenght()) * vec;
		//}

		/// <summary>
		/// Obtiene el producto vectorial entre este vector y el vector 'vec'.
		/// </summary>
		Vector3_t Cross(const Vector3_t& vec) const {
			return Vector3_t(y * vec.Z - Z * vec.y, Z * vec.x - x * vec.Z, x * vec.y - y * vec.x);
		}

		/// <summary>
		/// Devuelve un vector 3D normalizado con la misma dirección que este.
		/// </summary>
		Vector3_t GetNormalized() const {
			T length = GetLenght();

			return Vector3_t(x / length, y / length, Z / length);
		}

		/// <summary>
		/// Normaliza el vector 3D para que su módulo (GetLength()) sea 1.
		/// </summary>
		void Normalize() {
			T length = GetLenght();

			x /= length;
			y /= length;
			Z /= length;
		}

		/// <summary>
		/// OSK::Vector2 a glm::vec3.
		/// </summary>
		glm::vec3 ToGLM() const {
			return glm::vec3(x, y, Z);
		}

		/// <summary>
		/// OSK::Vector2 a glm::vec.
		/// </summary>
		inline glm::vec<3, T> ToGLM_T() const {
			return glm::vec<3, T>(x, y, Z);
		}

		/// <summary>
		/// Devuelve un vector con variables de distinto tipo.
		/// P = nuevo tipo de las variables.
		/// </summary>
		template <typename P> inline Vector3_t<P> ToVec3() const {
			return Vector3_t<P>(x, y, Z);
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

		inline std::string ToString() const {
			return "{ " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(Z) + " }";
		}

		/// <summary>
		/// Representa la primera coordenada.
		/// </summary>
		T x;

		/// <summary>
		/// Representa la segunda coordenada.
		/// </summary>
		T y;

		/// <summary>
		/// Representa la tercera coordenada.
		/// </summary>
		T Z;

	};

	/// <summary>
	/// Precisión = float.
	/// </summary>
	/// 
	/// @deprecated Usar Vector3f.
	using Vector3 = Vector3_t<float_t>;
	
	/// <summary>
	/// Precisión = float.
	/// </summary>
	using Vector3f = Vector3_t<float_t>;
	
	/// <summary>
	/// Precisión = double.
	/// </summary>
	using Vector3d = Vector3_t<double_t>;
	
	/// <summary>
	/// Precisión = int32.
	/// </summary>
	using Vector3i = Vector3_t<int32_t>;
	
	/// <summary>
	/// Precisión = uint32.
	/// </summary>
	using Vector3ui = Vector3_t<uint32_t>;

	template <> inline std::string ToString<Vector3f>(const Vector3f& value) {
		return "{ " + std::to_string(value.x) + ", " + std::to_string(value.y) + ", " + std::to_string(value.Z) + " }";
	}

}
