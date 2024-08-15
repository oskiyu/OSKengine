#pragma once

#include <glm/glm.hpp>

#include "Vector2.hpp"

namespace OSK {

	/// <summary>
	/// Representa un vector con 4 coordenadas (Rectangle).
	/// </summary>
	/// <typeparam name="T">Precisi�n.</typeparam>
	template <typename T> class Vector4_t {

	public:

		/// @brief Tipo de dato num�rico usado 
		/// para almacenar los n�meros.
		using TNumericType = T;

	public:

		/// <summary>
		/// Crea un vector 4D nulo { 0, 0, 0, 0 }.
		/// </summary>
		Vector4_t() {
			this->x = (T)0;
			this->y = (T)0;
			this->z = (T)0;
			this->w = (T)0;
		}

		/// <summary>
		/// Crea un vector 4D.
		/// </summary>
		Vector4_t(const T& x, const T& y, const T& z, const T& w) {
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

		/// <summary>
		/// Crea una instancia del Vector2 en la que X, Y, Z y W tienen en mismo valor.
		/// </summary>
		/// <param name="value">Valor para X, Y, Z y W.</param>
		Vector4_t(const T& value) {
			this->x = value;
			this->y = value;
			this->z = value;
			this->w = value;
		}

		/// <summary>
		/// Crea un vector 4D con los par�metros de vec.
		/// </summary>
		Vector4_t(const glm::vec4& vec) {
			this->x = vec.x;
			this->y = vec.y;
			this->z = vec.z;
			this->w = vec.w;
		}

		/// <summary>
		/// Operaci�n Vector4 + Vector4.
		/// 
		/// @note X1 + X2; Y1 + Y2, Z1 + Z2, W1 + W2.
		/// </summary>
		Vector4_t operator+(const Vector4_t& vec) const {
			return Vector4_t(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
		}

		/// <summary>
		/// Operaci�n Vector4 += Vector4.
		/// 
		/// @note X1 + X2; Y1 + Y2, Z1 + Z2, W1 + W2.
		/// @note Modifica el vector.
		/// </summary>
		Vector4_t& operator+=(const Vector4_t& vec) {
			*this = operator+(vec);
			
			return *this;
		}

		/// <summary>
		/// Negaci�n del Vector4.
		/// 
		/// @note -X; -Y; -Z; -W.
		/// </summary>
		Vector4_t operator-() const {
			return Vector4_t(-x, -y, -z, -w);
		}

		/// <summary>
		/// Operaci�n Vector4 - Vector4.
		/// 
		/// @note X1 - X2; Y1 - Y2; Z1 - Z2, W1 - W2.
		/// </summary>
		Vector4_t operator-(const Vector4_t& vec) const {
			return Vector4_t(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
		}

		/// <summary>
		/// Operaci�n Vector4 -= Vector4.
		/// 
		/// @note X1 - X2; Y1 - Y2; Z1 - Z2, W1 - W2.
		/// @note Modifica el vector.
		/// </summary>
		Vector4_t& operator-=(const Vector4_t& vec) {
			*this = operator-(vec);

			return *this;
		}

		/// <summary>
		/// Operaci�n Vector4 * Vector4.
		/// 
		/// @note X1 * X2; Y1 * Y2; Z1 * Z2; W1 * W2.
		/// </summary>
		Vector4_t operator*(const Vector4_t& vec) const {
			return Vector4_t(x * vec.x, y * vec.y, z * vec.z, w * vec.w);
		}

		/// <summary>
		/// Operaci�n Vector4 *= Vector4.
		/// 
		/// @note X1 * X2; Y1 * Y2; Z1 * Z2, W1 * W2.
		/// @note Modifica el vector.
		/// </summary>
		Vector4_t& operator*=(const Vector4_t& vec) {
			*this = operator*(vec);

			return *this;
		}

		/// <summary>
		/// Operaci�n Vector4 * float.
		/// 
		/// @note X * value; Y * value; Z * value; W * value.
		/// </summary>
		Vector4_t operator*(const T& value) const {
			return Vector4_t(x * value, y * value, z * value, w * value);
		}

		/// <summary>
		/// Operaci�n Vector4 *= T.
		/// 
		/// @note X1 * value; Y1 * value; Z1 * value, W1 * value.
		/// @note Modifica el vector.
		/// </summary>
		Vector4_t& operator*=(const T& value) {
			*this = operator*(value);

			return *this;
		}

		/// <summary>
		/// Operaci�n Vector4 / Vector4.
		/// 
		/// @note X / vec.X; Y / vec.Y, Z / vec.Z, W / vec.W.
		/// </summary>
		Vector4_t operator/(const Vector4_t& vec) const {
			return Vector4_t(x / vec.x, y / vec.y, z / vec.z, w / vec.w);
		}

		/// <summary>
		/// Operaci�n Vector4 /= Vector4.
		/// 
		/// @note X1 / X2; Y1 / Y2; Z1 / Z2, W1 / W2.
		/// @note Modifica el vector.
		/// </summary>
		Vector4_t& operator/=(const Vector4_t& vec) {
			*this = operator/(vec);

			return *this;
		}

		/// <summary>
		/// Operaci�n Vector4 / float_t.
		/// 
		/// @note X / value; Y / value; Z / value; W / value.
		/// </summary>
		Vector4_t operator/(const T& value) const {
			return Vector4_t(x / value, y / value, z / value, w / value);
		}

		/// <summary>
		/// Operaci�n Vector4 /= T.
		/// 
		/// @note X1 / value; Y1 / value; Z1 / value, W1 / value.
		/// @note Modifica el vector.
		/// </summary>
		Vector4_t& operator/=(const T& value) {
			*this = operator/(value);

			return *this;
		}

		/// <summary>
		/// Comparaci�n.
		/// True si todos los componentes son iguales.
		/// </summary>
		bool operator==(const Vector4_t& vec) const {
			return x == vec.x && y == vec.y && z == vec.z && w == vec.w;
		}

		/// <summary>
		/// Comparaci�n.
		/// True si no todos los componentes son iguales.
		/// </summary>
		bool operator!=(const Vector4_t& vec) const {
			return !operator==(vec);
		}

		/// <summary>
		/// Obtiene la posici�n del rect�ngulo (X, Y).
		/// </summary>
		Vector2_t<T> GetRectanglePosition() const {
			return Vector2_t<T>(x, y);
		}

		/// <summary>
		/// Obtiene el tama�o del rect�ngulo (Z, W).
		/// </summary>
		Vector2_t<T> GetRectangleSize() const {
			return Vector2_t<T>(z, w);
		}

		/// <summary>
		/// Obtiene la anchura del rect�ngulo (Z).
		/// </summary>
		inline T GetRectangleWidth() const {
			return z;
		}

		/// <summary>
		/// Obtiene la altura del rect�ngulo (W).
		/// </summary>
		inline T GetRectangleHeight() const {
			return w;
		}

		/// <summary>
		/// Obtiene el tope del rect�ngulo (Y + W).
		/// </summary>
		inline T GetRectangleTop() const {
			return y + w;
		}

		/// <summary>
		/// Obtiene el lateral derecho del tect�ngulo (X + Z).
		/// </summary>
		inline T GetRectangleRight() const {
			return x + z;
		}

		/// <summary>
		/// Punto central del rect�ngulo.
		/// </summary>
		Vector2_t<T> GetRectangleMiddlePoint() const {
			return Vector2_t<T>(x + GetRectangleWidth() / 2, y + GetRectangleHeight() / 2);
		}

		/// <summary>
		/// Retorna 'true' si los dos rect�ngulos se tocan.
		/// </summary>
		inline bool Intersects(const Vector4_t& vec) const {
			return x < vec.x + vec.z && x + z > vec.x && y < vec.y + vec.w && w + y > vec.y;
		}

		/// @brief Comrpueba si un punto est� dentro del rect�ngulo.
		/// @param point Posici�n del punto.
		/// @return True si est� contenido dentro del rect�ngulo.
		inline constexpr bool ContainsPoint(const Vector2_t<T>& point) const {
			return x <= point.x && y <= point.y && x + z >= point.x && y + w >= point.y;
		}

		/// <summary>
		/// OSK::Vector4 a glm::vec4.
		/// </summary>
		inline glm::vec4 ToGLM() const {
			return glm::vec4(x, y, z, w);
		}

		/// <summary>
		/// OSK::Vector4 a glm::vec.
		/// </summary>
		inline glm::vec<4, T> ToGLM_T() const {
			return glm::vec<4, T>(x, y, z, w);
		}

		/// <summary>
		/// Devuelve un vector con variables de distinto tipo.
		/// P = nuevo tipo de las variables.
		/// </summary>
		template <typename P> inline Vector4_t<P> ToVec4() const {
			return Vector4_t<P>((P)x, (P)y, (P)z, (P)w);
		}

		/// <summary>
		/// Devuelve un Vector4f a partir de este.
		/// </summary>
		inline Vector4_t<float_t> ToVector4f() const {
			return ToVec4<float>();
		}

		/// <summary>
		/// Devuelve un Vector4d a partir de este.
		/// </summary>
		inline Vector4_t<double_t> ToVector4d() const {
			return ToVec4<double_t>();
		}

		/// <summary>
		/// Devuelve un Vector4i a partir de este.
		/// </summary>
		inline Vector4_t<int32_t> ToVector4i() const {
			return ToVec4<int32_t>();
		}

		/// <summary>
		/// Devuelve un Vector4ui a partir de este.
		/// </summary>
		inline Vector4_t<uint32_t> ToVector4ui() const {
			return ToVec4<uint32_t>();
		}

		/// <summary>
		/// Primera coordenada.
		/// Posici�n X del rect�ngulo.
		/// </summary>
		T x;

		/// <summary>
		/// Segunda coordenada.
		/// Posici�n Y del rect�ngulo.
		/// </summary>
		T y;

		/// <summary>
		/// Tercera coordenada.
		/// Ancho del rect�ngulo.
		/// </summary>
		T z;

		/// <summary>
		/// Cuarta coordenada.
		/// Alto del rect�ngulo.
		/// </summary>
		T w;

	};

	/// <summary>
	/// Precisi�n = float.
	/// </summary>
	/// 
	/// @deprecated Usar Vector4f.
	typedef Vector4_t<float_t> Vector4;

	/// <summary>
	/// Precisi�n = float.
	/// </summary>
	typedef Vector4_t<float_t> Vector4f;
	
	/// <summary>
	/// Precisi�n = double.
	/// </summary>
	typedef Vector4_t<double_t> Vector4d;
	
	/// <summary>
	/// Precisi�n = int32.
	/// </summary>
	typedef Vector4_t<int32_t> Vector4i;
	
	/// <summary>
	/// Precisi�n = uint32.
	/// </summary>
	typedef Vector4_t<uint32_t> Vector4ui;

}
