#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "Log.h"

#include <glm.hpp>

#include "Vector2.hpp"

namespace OSK {

	/// <summary>
	/// Representa un vector con 4 coordenadas (Rectangle).
	/// </summary>
	/// <typeparam name="T">Precisi�n.</typeparam>
	template <typename T> class Vector4_t {

	public:

		/// <summary>
		/// Crea un vector 4D nulo { 0, 0, 0, 0 }.
		/// </summary>
		Vector4_t() {
			X = (T)0;
			Y = (T)0;
			Z = (T)0;
			W = (T)0;
		}

		/// <summary>
		/// Crea un vector 4D.
		/// </summary>
		Vector4_t(const T& x, const T& y, const T& z, const T& w) {
			X = x;
			Y = y;
			Z = z;
			W = w;
		}

		/// <summary>
		/// Crea una instancia del Vector2 en la que X, Y, Z y W tienen en mismo valor.
		/// </summary>
		/// <param name="value">Valor para X, Y, Z y W.</param>
		Vector4_t(const T& value) {
			X = value;
			Y = value;
			Z = value;
			W = value;
		}

		/// <summary>
		/// Crea un vector 4D con los par�metros de vec.
		/// </summary>
		Vector4_t(const glm::vec4& vec) {
			X = vec.x;
			Y = vec.y;
			Z = vec.z;
			W = vec.w;
		}

		/// <summary>
		/// Operaci�n Vector4 + Vector4.
		/// X1 + X2; Y1 + Y2, Z1 + Z2, W1 + W2.
		/// </summary>
		Vector4_t operator+(const Vector4_t& vec) const {
			return Vector4_t(X + vec.X, Y + vec.Y, Z + vec.Z, W + vec.W);
		}

		/// <summary>
		/// Operaci�n Vector4 += Vector4.
		/// X1 + X2; Y1 + Y2, Z1 + Z2, W1 + W2.
		/// Modifica el vector.
		/// </summary>
		Vector4_t& operator+=(const Vector4_t& vec) {
			*this = operator+(vec);
			
			return *this;
		}

		/// <summary>
		/// Negaci�n del Vector4.
		/// -X; -Y; -Z; -W.
		/// </summary>
		Vector4_t operator-() const {
			return Vector4_t(-X, -Y, -Z, -W);
		}

		/// <summary>
		/// Operaci�n Vector4 - Vector4.
		/// X1 - X2; Y1 - Y2; Z1 - Z2, W1 - W2.
		/// </summary>
		Vector4_t operator-(const Vector4_t& vec) const {
			return Vector4_t(X - vec.X, Y - vec.Y, Z - vec.Z, W - vec.W);
		}

		/// <summary>
		/// Operaci�n Vector4 -= Vector4.
		/// X1 - X2; Y1 - Y2; Z1 - Z2, W1 - W2.
		/// Modifica el vector.
		/// </summary>
		Vector4_t& operator-=(const Vector4_t& vec) {
			*this = operator-(vec);

			return *this;
		}

		/// <summary>
		/// Operaci�n Vector4 * Vector4.
		/// X1 * X2; Y1 * Y2; Z1 * Z2; W1 * W2.
		/// </summary>
		Vector4_t operator*(const Vector4_t& vec) const {
			return Vector4_t(X * vec.X, Y * vec.Y, Z * vec.Z, W * vec.W);
		}

		/// <summary>
		/// Operaci�n Vector4 *= Vector4.
		/// X1 * X2; Y1 * Y2; Z1 * Z2, W1 * W2.
		/// Modifica el vector.
		/// </summary>
		Vector4_t& operator*=(const Vector4_t& vec) {
			*this = operator*(vec);

			return *this;
		}

		/// <summary>
		/// Operaci�n Vector4 * float.
		/// X * value; Y * value; Z * value; W * value.
		/// </summary>
		Vector4_t operator*(const T& value) const {
			return Vector4_t(X * value, Y * value, Z * value, W * value);
		}

		/// <summary>
		/// Operaci�n Vector4 *= T.
		/// X1 * value; Y1 * value; Z1 * value, W1 * value.
		/// Modifica el vector.
		/// </summary>
		Vector4_t& operator*=(const T& value) {
			*this = operator*(value);

			return *this;
		}

		/// <summary>
		/// Operaci�n Vector4 / Vector4.
		/// X / vec.X; Y / vec.Y, Z / vec.Z, W / vec.W.
		/// </summary>
		Vector4_t operator/(const Vector4_t& vec) const {
			return Vector4_t(X / vec.X, Y / vec.Y, Z / vec.Z, W / vec.W);
		}

		/// <summary>
		/// Operaci�n Vector4 /= Vector4.
		/// X1 / X2; Y1 / Y2; Z1 / Z2, W1 / W2.
		/// Modifica el vector.
		/// </summary>
		Vector4_t& operator/=(const Vector4_t& vec) {
			*this = operator/(vec);

			return *this;
		}

		/// <summary>
		/// Operaci�n Vector4 / float_t.
		/// X / value; Y / value; Z / value; W / value.
		/// </summary>
		Vector4_t operator/(const T& value) const {
			return Vector4_t(X / value, Y / value, Z / value, W / value);
		}

		/// <summary>
		/// Operaci�n Vector4 /= T.
		/// X1 / value; Y1 / value; Z1 / value, W1 / value.
		/// Modifica el vector.
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
			return X == vec.X && Y == vec.Y && Z == vec.Z && W == vec.W;
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
			return Vector2_t<T>(X, Y);
		}

		/// <summary>
		/// Obtiene el tama�o del rect�ngulo (Z, W).
		/// </summary>
		Vector2_t<T> GetRectangleSize() const {
			return Vector2_t<T>(Z, W);
		}

		/// <summary>
		/// Obtiene la anchura del rect�ngulo (Z).
		/// </summary>
		inline T GetRectangleWidth() const {
			return Z;
		}

		/// <summary>
		/// Obtiene la altura del rect�ngulo (W).
		/// </summary>
		inline T GetRectangleHeight() const {
			return W;
		}

		/// <summary>
		/// Obtiene el tope del rect�ngulo (Y + W).
		/// </summary>
		inline T GetRectangleTop() const {
			return Y + W;
		}

		/// <summary>
		/// Obtiene el lateral derecho del tect�ngulo (X + Z).
		/// </summary>
		inline T GetRectangleRight() const {
			return X + Z;
		}

		/// <summary>
		/// Punto central del rect�ngulo.
		/// </summary>
		Vector2_t<T> GetRectangleMiddlePoint() const {
			return Vector2_t<T>(X + GetRectangleWidth() / 2, Y + GetRectangleHeight() / 2);
		}

		/// <summary>
		/// Retorna 'true' si los dos rect�ngulos se tocan.
		/// </summary>
		inline bool Intersects(const Vector4_t& vec) const {
			return X < vec.X + vec.Z && X + Z > vec.X && Y < vec.Y + vec.W && W + Y > vec.Y;
		}

		/// <summary>
		/// OSK::Vector4 a glm::vec4.
		/// </summary>
		inline glm::vec4 ToGLM() const {
			return glm::vec4(X, Y, Z, W);
		}

		/// <summary>
		/// OSK::Vector4 a glm::vec.
		/// </summary>
		inline glm::vec<4, T> ToGLM_T() const {
			return glm::vec<4, T>(X, Y, Z, W);
		}

		/// <summary>
		/// Devuelve un vector con variables de distinto tipo.
		/// P = nuevo tipo de las variables.
		/// </summary>
		template <typename P> inline Vector4_t<P> ToVec4() const {
			return Vector4_t<P>((P)X, (P)Y, (P)Z, (P)W);
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
		T X;

		/// <summary>
		/// Segunda coordenada.
		/// Posici�n Y del rect�ngulo.
		/// </summary>
		T Y;

		/// <summary>
		/// Tercera coordenada.
		/// Ancho del rect�ngulo.
		/// </summary>
		T Z;

		/// <summary>
		/// Cuarta coordenada.
		/// Alto del rect�ngulo.
		/// </summary>
		T W;

	};

	/// <summary>
	/// Precisi�n = float.
	/// </summary>
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
