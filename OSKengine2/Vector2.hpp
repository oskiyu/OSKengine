#pragma once

#include <glm.hpp>

namespace OSK {

	/// <summary>
	/// Representa un vector 2D.
	/// </summary>
	/// <typeparam name="T">Precisi�n.</typeparam>
	template <typename T> class Vector2_t {

	public:

		/// <summary>
		/// Crea un vector 2D nulo { 0, 0 }.
		/// </summary>
		Vector2_t() {
			X = 0;
			Y = 0;
		}

		/// <summary>
		/// Crea una instancia del Vector2.
		/// </summary>
		/// <param name="x">X.</param>
		/// <param name="y">Y.</param>
		Vector2_t(const T& x, const T& y) {
			X = x;
			Y = y;
		}

		/// <summary>
		/// Crea una instancia del Vector2 en la que X e Y tienen en mismo valor.
		/// </summary>
		/// <param name="value">X e Y.</param>
		Vector2_t(const T& value) {
			X = value;
			Y = value;
		}

		/// <summary>
		/// Crea un vector 2D con los par�metros de vec.
		/// </summary>
		/// <param name="vec">Copia el vetor.</param>
		Vector2_t(const glm::vec2& vec) {
			X = vec.x;
			Y = vec.y;
		}

		/// <summary>
		/// Operaci�n Vector2 + Vector2.
		/// 
		/// @note X1 + X2; Y1 + Y2.
		/// </summary>
		/// <param name="vec">Otro vector.</param>
		/// <returns>Suma.</returns>
		inline Vector2_t operator+(const Vector2_t& vec) const {
			return Vector2_t(X + vec.X, Y + vec.Y);
		}

		/// <summary>
		/// Operaci�n Vector2 += Vector2.
		/// 
		/// @note X1 + X2; Y1 + Y2.
		/// @note Modifica este vector.
		/// </summary>
		/// <param name="vec">Otro vector.</param>
		/// <returns>Suma.</returns>
		inline Vector2_t& operator+=(const Vector2_t& vec) {
			X += vec.X;
			Y += vec.Y;

			return *this;
		}
		
		/// <summary>
		/// Negaci�n del Vector2.
		/// 
		/// @note -X; -Y.
		/// </summary>
		/// <returns>Negado.</returns>
		inline Vector2_t operator-() const {
			return Vector2_t(-X, -Y);
		}

		/// <summary>
		/// Operaci�n Vector2 - Vector2.
		/// 
		/// @note X1 - X2; Y1 - Y2.
		/// </summary>
		/// <param name="vec"></param>
		/// <returns></returns>
		inline Vector2_t operator-(const Vector2_t& vec) const {
			return Vector2_t(X - vec.X, Y - vec.Y);
		}

		/// <summary>
		/// Operaci�n Vector2 -= Vector2.
		/// 
		/// @note X1 - X2; Y1 - Y2.
		/// @note Modifica este vector.
		/// </summary>
		/// <param name="vec">Otro vector.</param>
		/// <returns>Resto.</returns>
		inline Vector2_t& operator-=(const Vector2_t& vec) {
			X -= vec.X;
			Y -= vec.Y;

			return *this;
		}
		
		/// <summary>
		/// Operaci�n Vector2 * Vector2.
		/// 
		/// @note X1 * X2; Y1 * Y2.
		/// </summary>
		/// <param name="vec">Otro vector.</param>
		/// <returns>Multiplicaci�n.</returns>
		inline Vector2_t operator*(const Vector2_t& vec) const {
			return Vector2_t(X * vec.X, Y * vec.Y);
		}

		/// <summary>
		/// Operaci�n Vector2 *= Vector2.
		/// 
		/// @note X1 * X2; Y1 * Y2.
		/// @note Modifica este vector.
		/// </summary>
		/// <param name="vec">Otro vector.</param>
		/// <returns>Multiplicaci�n.</returns>
		inline Vector2_t& operator*=(const Vector2_t& vec) {
			X *= vec.X;
			Y *= vec.Y;

			return *this;
		}
		
		/// <summary>
		/// Operaci�n Vector2 * float_t.
		/// 
		/// @note X * value; Y * value.
		/// </summary>
		/// <param name="value">Valor.</param>
		/// <returns>Multiplicaci�n.</returns>
		inline Vector2_t operator*(const T& value) const {
			return Vector2_t(X * value, Y * value);
		}

		/// <summary>
		/// Operaci�n Vector2 * float_t.
		/// 
		/// @note X * value; Y * value.
		/// @note Modifica este vector.
		/// </summary>
		/// <param name="value">Valor.</param>
		/// <returns>Multiplicaci�n.</returns>
		inline Vector2_t& operator*=(const T& value) {
			X *= value;
			Y *= value;

			return *this;
		}

		/// <summary>
		/// Operaci�n Vector2 / float_t.
		/// 
		/// @note X = value; Y = value.
		/// </summary>
		inline Vector2_t operator/(const T& value) const {
			return Vector2_t(X / value, Y / value);
		}

		/// <summary>
		/// Operaci�n Vector2 /= Vector2.
		/// 
		/// @note X = vec.X; Y = vec.Y.
		/// @note Modifica este vector.
		/// </summary>
		/// <param name="vec">Otro vector.</param>
		/// <returns>Divisi�n.</returns>
		inline Vector2_t& operator/=(const Vector2_t& vec) {
			X /= vec.X;
			Y /= vec.Y;

			return *this;
		}

		/// <summary>
		/// Operaci�n Vector2 / Vector2.
		/// 
		/// @note X = vec.X; Y = vec.Y.
		/// </summary>
		inline Vector2_t operator/(const Vector2_t& vec) const {
			return Vector2_t(X / vec.X, Y / vec.Y);
		}

		/// <summary>
		/// Operaci�n Vector2 /= float_t.
		/// 
		/// @note X = value; Y = value.
		/// @note Modifica este vector.
		/// </summary>
		inline Vector2_t& operator/=(const T& value) {
			X /= value;
			Y /= value;

			return *this;
		}

		/// <summary>
		/// Comparaci�n.
		/// True si todos los componentes son iguales.
		/// </summary>
		bool operator==(const Vector2_t& vec) const {
			return X == vec.X && Y == vec.Y;
		}

		/// <summary>
		/// Comparaci�n.
		/// True si no todos los componentes son iguales.
		/// </summary>
		bool operator!=(const Vector2_t& vec) const {
			return !operator==(vec);
		}

		/// <summary>
		/// M�dulo del vector.
		/// Obtiene la longitud del vector.
		/// </summary>
		inline T GetLength() const {
			return glm::sqrt(X * X + Y * Y);
		}

		/// <summary>
		/// Obtiene la distancia entre la posici�n representada por este vector y por el vector 'vec'.
		/// </summary>
		inline T GetDistanceTo(const Vector2_t& vec) const {
			T dx = X - vec.X;
			T dy = Y - vec.Y;

			return glm::sqrt(dx * dx + dy * dy);
		}

		/// <summary>
		/// Producto escalar de dos vectores.
		/// </summary>
		inline T Dot(const Vector2_t& vec) const {
			return X * vec.X + Y * vec.Y;
		}

		/// <summary>
		/// Devuelve un vector 2D normalizado con la misma direcci�n que este.
		/// </summary>
		Vector2_t GetNormalized() const {
			T length = GetLength();

			return Vector2_t(X / length, Y / length);
		}

		/// <summary>
		/// Normaliza este vector 2D para que su m�dulo (GetLength()) sea 1.
		/// </summary>
		void Normalize() {
			T length = GetLength();

			X /= length;
			Y /= length;
		}

		/// <summary>
		/// OSK::Vector2 a glm::vec2.
		/// </summary>
		inline glm::vec2 ToGLM() const {
			return glm::vec2(X, Y);
		}

		/// <summary>
		/// OSK::Vector2 a glm::vec.
		/// </summary>
		inline glm::vec<2, T> ToGLM_T() const {
			return glm::vec<2, T>(X, Y);
		}

		/// <summary>
		/// Devuelve un vector con variables de distinto tipo.
		/// P = nuevo tipo de las variables.
		/// </summary>
		template <typename P> inline Vector2_t<P> ToVec2() const {
			return Vector2_t<P>((P)X, (P)Y);
		}

		/// <summary>
		/// Devuelve un Vector2f a partir de este.
		/// </summary>
		inline Vector2_t<float_t> ToVector2f() const {
			return ToVec2<float>();
		}

		/// <summary>
		/// Devuelve un Vector2d a partir de este.
		/// </summary>
		inline Vector2_t<double_t> ToVector2d() const {
			return ToVec2<double_t>();
		}

		/// <summary>
		/// Devuelve un Vector2i a partir de este.
		/// </summary>
		inline Vector2_t<int32_t> ToVector2i() const {
			return ToVec2<int32_t>();
		}

		/// <summary>
		/// Devuelve un Vector2ui a partir de este.
		/// </summary>
		inline Vector2_t<uint32_t> ToVector2ui() const {
			return ToVec2<uint32_t>();
		}

		/// <summary>
		/// Representa la primera coordenada.
		/// </summary>
		T X;

		/// <summary>
		/// Representa la segunda coordenada.
		/// </summary>
		T Y;

	};

	/// <summary>
	/// Precisi�n = float.
	/// </summary>
	/// 
	/// @deprecated
	typedef Vector2_t<float_t> Vector2;

	/// <summary>
	/// Precisi�n = float.
	/// </summary>
	typedef Vector2_t<float_t> Vector2f;

	/// <summary>
	/// Precisi�n = double.
	/// </summary>
	typedef Vector2_t<double_t> Vector2d;

	/// <summary>
	/// Precisi�n = int32.
	/// </summary>
	typedef Vector2_t<int32_t> Vector2i;

	/// <summary>
	/// Precisi�n = uint32.
	/// </summary>
	typedef Vector2_t<uint32_t> Vector2ui;

}
