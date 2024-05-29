#pragma once

#include "ApiCall.h"
#include "ToString.h"

#include <glm/glm.hpp>


namespace OSK {

	/// @brief Representa un vector 3D.
	/// @tparam T Precisión / tipo de dato.
	template <typename T> class Vector3_t {

	public:

		static OSKAPI_CALL Vector3_t const Zero;
		static OSKAPI_CALL Vector3_t const One;

	public:

		/// @brief Crea un vector 3D nulo { 0, 0, 0 }.
		Vector3_t() = default;

		/// @brief Crea un vector 3D.
		/// @param x Primera coordenada.
		/// @param y Segunda coordenada.
		/// @param z Tercera coordenada.
		Vector3_t(const T& x, const T& y, const T& z) : x(x), y(y), z(z) {

		}

		/// @brief Crea una instancia del Vector2 en la que X, Y y Z tienen en mismo valor.
		/// @param value Valor para X, Y y Z.
		explicit Vector3_t(const T& value) : x(value), y(value), z(value) {

		}

		/// @brief Crea un vector equivalente a @p vec.
		explicit Vector3_t(const glm::vec3& vec) : x(vec.x), y(vec.y), z(vec.z) {

		}


		/// @param vec Otro vector.
		/// @return Suma de dos vectores.
		Vector3_t operator+(const Vector3_t& vec) const {
			return Vector3_t(x + vec.x, y + vec.y, z + vec.z);
		}

		/// @param vec Otro vector.
		/// @return Vector actual sumándole @p vec.
		Vector3_t& operator+=(const Vector3_t& vec) {
			x += vec.x;
			y += vec.y;
			z += vec.z;
			return *this;
		}


		/// @return Vector negado.
		Vector3_t operator-() const {
			return Vector3_t(-x, -y, -z);
		}

		/// @return Este vector menos @p vec.
		Vector3_t operator-(const Vector3_t& vec) const {
			return Vector3_t(x - vec.x, y - vec.y, z - vec.z);
		}

		/// @brief Resta @p vec a este vector.
		Vector3_t& operator-=(const Vector3_t& vec) {
			x -= vec.x;
			y -= vec.y;
			z -= vec.z;

			return *this;
		}


		/// @brief Multiplica cada una de las coordenadas.
		/// @param vec Otro vector.
		/// @return X1 * X2; Y1 * Y2; Z1 * Z2.
		Vector3_t operator*(const Vector3_t& vec) const {
			return Vector3_t(x * vec.x, y * vec.y, z * vec.z);
		}

		/// @brief Multiplica cada una de las coordenadas de este
		/// vector por las coordenadas del vector @p vec.
		/// @return Self (X1 * X2; Y1 * Y2, Z1 * Z2).
		Vector3_t& operator*=(const Vector3_t& vec) {
			x *= vec.x;
			y *= vec.y;
			z *= vec.z;
			return *this;
		}

		/// @brief Multiplica todas las coordenadas por el valor.
		/// @return X * value; Y * value; Z * value.
		Vector3_t operator*(const T& value) const {
			return Vector3_t(x * value, y * value, z * value);
		}

		/// @brief Multiplica todas las coordenadas por el valor.
		/// @return Self (X * value; Y * value; Z * value).
		Vector3_t& operator*=(const T& value) {
			x *= value;
			y *= value;
			z *= value;
			return *this;
		}


		/// @brief Divide cada una de las coordenadas.
		/// @return  X / vec.X; Y / vec.Y, Z / vec.Z.
		Vector3_t operator/(const Vector3_t& vec) const {
			return Vector3_t(x / vec.x, y / vec.y, z / vec.z);
		}

		/// @brief Divide cada una de las coordenadas.
		/// @return Self (X / vec.X; Y / vec.Y, Z / vec.Z).
		Vector3_t& operator/=(const Vector3_t& vec) {
			x /= vec.x;
			y /= vec.y;
			z /= vec.z;
			return *this;
		}

		/// @brief Divide todas las coordenadas por el valor.
		/// @return X / value; Y / value; Z / value.
		Vector3_t operator/(const T& value) const {
			return Vector3_t(x / value, y / value, z / value);
		}

		/// @brief Divide todas las coordenadas por el valor.
		/// @return Self (X / value; Y / value; Z / value).
		Vector3_t& operator/=(const T& value) {
			x /= value;
			y /= value;
			z /= value;
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
				glm::abs(z - other.z) < epsilon;
		}


		/// @return True si todos los componentes son iguales.
		bool operator==(const Vector3_t&) const = default;


		/// @return Longitud / módulo del vector.
		T GetLenght() const {
			return glm::sqrt(x * x + y * y + z * z);
		}


		/// @return Distancia entre la posición representada por este vector y por el vector @p vec..
		T GetDistanceTo(const Vector3_t& vec) const {
			const T dx = x - vec.x;
			const T dy = y - vec.y;
			const T dz = z - vec.z;

			return glm::sqrt(dx * dx + dy * dy + dz * dz);
		}

		/// @return Obtiene la distancia entre la posición representada por este vector y por el vector @p vec, al cuadrado.
		T GetDistanceTo2(const Vector3_t& vec) const {
			T dx = x - vec.x;
			T dy = y - vec.y;
			T dz = z - vec.z;

			return dx * dx + dy * dy + dz * dz;
		}


		/// @return Producto escalar entre este vector y el vector @p vec.
		T Dot(const Vector3_t& vec) const {
			return x * vec.x + y * vec.y + z * vec.z;
		}

		/// @return Producto vectorial entre este vector y @p vec.
		Vector3_t Cross(const Vector3_t& vec) const {
			return Vector3_t(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
		}

		
		/// @return Ángulo, en grados, entre este vector y el vector dado.
		/// Rango [0, 180].
		T GetAngle(const Vector3_t& vec) const {
			const float num = this->Dot(vec);
			const float den = this->GetLenght() * vec.GetLenght();

			float _cos = num / den;
			if (_cos > 1.0f) {
				_cos = 1.0f;
			}
			else if (_cos < -1.0f) {
				_cos = -1.0f;
			}

			return glm::degrees(glm::acos(_cos));
		}


		/// @return Vector normalizado con la misma dirección que este.
		Vector3_t GetNormalized() const {
			T length = GetLenght();
			return Vector3_t(x / length, y / length, z / length);
		}

		/// @brief Normaliza este vector.
		void Normalize() {
			T length = GetLenght();

			x /= length;
			y /= length;
			z /= length;
		}


		/// @return Convierte a vector GLM.
		glm::vec3 ToGlm() const {
			return glm::vec3(x, y, z);
		}


		/// @tparam P Tipo de dato (precisión).
		/// @return Vector con tipo de dato distinto.
		template <typename P> inline Vector3_t<P> ToVec3() const {
			return Vector3_t<P>(x, y, z);
		}

		
		/// @return Vector con float.
		inline Vector3_t<float_t> ToVector3f() const {
			return ToVec3<float>();
		}

		/// @return Vector con double.
		inline Vector3_t<double_t> ToVector3d() const {
			return ToVec3<double_t>();
		}

		/// @return Vector con int.
		inline Vector3_t<int32_t> ToVector3i() const {
			return ToVec3<int32_t>();
		}

		/// @return Vector con unsigned int.
		inline Vector3_t<uint32_t> ToVector3ui() const {
			return ToVec3<uint32_t>();
		}

		inline std::string ToString() const {
			return "{ " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + " }";
		}

		inline bool IsNaN() const {
			return ::std::isnan(x) || ::std::isnan(y) || ::std::isnan(z);
		}

		/// @brief Representa la primera coordenada.
		T x;

		/// @brief Representa la segunda coordenada.
		T y;

		/// @brief Representa la tercera coordenada.
		T z;

	};


	/// @brief Precisión = float.
	using Vector3f = Vector3_t<float_t>;

	/// @brief Precisión = double.
	using Vector3d = Vector3_t<double_t>;

	/// @brief Precisión = int.
	using Vector3i = Vector3_t<int32_t>;

	/// @brief Precisión = unsigned int.
	using Vector3ui = Vector3_t<uint32_t>;


	template <> inline std::string ToString<Vector3f>(const Vector3f& value) {
		return value.ToString();
	}

}
