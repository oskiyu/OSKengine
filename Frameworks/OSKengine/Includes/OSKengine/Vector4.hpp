#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "Log.h"

#include <glm.hpp>

#include "Vector2.hpp"

namespace OSK {

	//Representa un vector con 4 coordenadas (Rectangle).
	template <typename T> class OSKAPI_CALL Vector4_t {

	public:

		//Crea un vector 4D nulo { 0, 0, 0, 0 }.
		Vector4_t() {
			X = (T)0;
			Y = (T)0;
			Z = (T)0;
			W = (T)0;
		}

		//Crea un vector 4D.
		Vector4_t(const T& x, const T& y, const T& z, const T& w) {
			X = x;
			Y = y;
			Z = z;
			W = w;
		}

		//Crea una instancia del Vector2 en la que X, Y, Z y W tienen en mismo valor.
		//<value>: valor para X, Y, Z y W.
		Vector4_t(const T& value) {
			X = value;
			Y = value;
			Z = value;
			W = value;
		}

		//Crea un vector 4D con los parámetros de vec.
		Vector4_t(const glm::vec4& vec) {
			X = vec.x;
			Y = vec.y;
			Z = vec.z;
			W = vec.w;
		}

		//Operación Vector4 + Vector4.
		//X1 + X2; Y1 + Y2, Z1 + Z2, W1 + W2.
		Vector4_t operator+(const Vector4_t& vec) const {
			return Vector4_t(X + vec.X, Y + vec.Y, Z + vec.Z, W + vec.W);
		}

		//Operación Vector4 += Vector4.
		//X1 + X2; Y1 + Y2, Z1 + Z2, W1 + W2.
		//Modifica el vector.
		Vector4_t& operator+=(const Vector4_t& vec) {
			*this = operator+(vec);
			
			return *this;
		}

		//Negación del Vector4.
		//-X; -Y; -Z; -W.
		Vector4_t operator-() const {
			return Vector4_t(-X, -Y, -Z, -W);
		}

		//Operación Vector4 - Vector4.
		//X1 - X2; Y1 - Y2; Z1 - Z2, W1 - W2.
		Vector4_t operator-(const Vector4_t& vec) const {
			return Vector4_t(X - vec.X, Y - vec.Y, Z - vec.Z, W - vec.W);
		}

		//Operación Vector4 -= Vector4.
		//X1 - X2; Y1 - Y2; Z1 - Z2, W1 - W2.
		//Modifica el vector.
		Vector4_t& operator-=(const Vector4_t& vec) {
			*this = operator-(vec);

			return *this;
		}

		//Operación Vector4 * Vector4.
		//X1 * X2; Y1 * Y2; Z1 * Z2; W1 * W2.
		Vector4_t operator*(const Vector4_t& vec) const {
			return Vector4_t(X * vec.X, Y * vec.Y, Z * vec.Z, W * vec.W);
		}

		//Operación Vector4 *= Vector4.
		//X1 * X2; Y1 * Y2; Z1 * Z2, W1 * W2.
		//Modifica el vector.
		Vector4_t& operator*=(const Vector4_t& vec) {
			*this = operator*(vec);

			return *this;
		}

		//Operación Vector4 * float.
		//X * value; Y * value; Z * value; W * value.
		Vector4_t operator*(const T& value) const {
			return Vector4_t(X * value, Y * value, Z * value, W * value);
		}

		//Operación Vector4 *= T.
		//X1 * value; Y1 * value; Z1 * value, W1 * value.
		//Modifica el vector.
		Vector4_t& operator*=(const T& value) {
			*this = operator*(value);

			return *this;
		}

		//Operación Vector4 / Vector4.
		//X / vec.X; Y / vec.Y, Z / vec.Z, W / vec.W.
		Vector4_t operator/(const Vector4_t& vec) const {
			return Vector4_t(X / vec.X, Y / vec.Y, Z / vec.Z, W / vec.W);
		}

		//Operación Vector4 /= Vector4.
		//X1 / X2; Y1 / Y2; Z1 / Z2, W1 / W2.
		//Modifica el vector.
		Vector4_t& operator/=(const Vector4_t& vec) {
			*this = operator/(vec);

			return *this;
		}

		//Operación Vector4 / float_t.
		//X / value; Y / value; Z / value; W / value.
		Vector4_t operator/(const T& value) const {
			return Vector4_t(X / value, Y / value, Z / value, W / value);
		}

		//Operación Vector4 /= T.
		//X1 / value; Y1 / value; Z1 / value, W1 / value.
		//Modifica el vector.
		Vector4_t& operator/=(const T& value) {
			*this = operator/(value);

			return *this;
		}

		//Comparación.
		//True si todos los componentes son iguales.
		bool operator==(const Vector4_t& vec) const {
			return X == vec.X && Y == vec.Y && Z == vec.Z && W == vec.W;
		}

		//Comparación.
		//True si no todos los componentes son iguales.
		bool operator!=(const Vector4_t& vec) const {
			return !operator==(vec);
		}

		//Obtiene la posición del rectángulo (X, Y).
		Vector2_t<T> GetRectanglePosition() const {
			return Vector2_t<T>(X, Y);
		}

		//Obtiene el tamaño del rectángulo (Z, W).
		Vector2_t<T> GetRectangleSize() const {
			return Vector2_t<T>(Z, W);
		}

		//Obtiene la anchura del rectángulo (Z).
		inline T GetRectangleWidth() const {
			return Z;
		}

		//Obtiene la altura del rectángulo (W).
		inline T GetRectangleHeight() const {
			return W;
		}

		//Obtiene el tope del rectángulo (Y + W).
		inline T GetRectangleTop() const {
			return Y + W;
		}

		//Obtiene el lateral derecho del tectángulo (X + Z).
		inline T GetRectangleRight() const {
			return X + Z;
		}

		//Punto central del rectángulo.
		Vector2_t<T> GetRectangleMiddlePoint() const {
			return Vector2_t<T>(X + GetRectangleWidth() / 2, Y + GetRectangleHeight() / 2);
		}

		//Retorna 'true' si los dos rectángulos se tocan.
		inline bool Intersects(const Vector4_t& vec) const {
			return X < vec.X + vec.Z && X + Z > vec.X && Y < vec.Y + vec.W && W + Y > vec.Y;
		}

		//OSK::Vector4 a glm::vec4.
		inline glm::vec4 ToGLM() const {
			return glm::vec4(X, Y, Z, W);
		}

		//OSK::Vector4 a glm::vec.
		inline glm::vec<4, T> ToGLM_T() const {
			return glm::vec<4, T>(X, Y, Z, W);
		}

		//Devuelve un vector con variables de distinto tipo.
		//P = nuevo tipo de las variables.
		template <typename P> inline Vector4_t<P> ToVec4() const {
			return Vector4_t<P>(X, Y, Z, W);
		}

		//Devuelve un Vector4f a partir de este.
		inline Vector4_t<float_t> ToVector4f() const {
			return ToVec4<float>();
		}

		//Devuelve un Vector4d a partir de este.
		inline Vector4_t<double_t> ToVector4d() const {
			return ToVec4<double_t>();
		}

		//Devuelve un Vector4i a partir de este.
		inline Vector4_t<int32_t> ToVector4i() const {
			return ToVec4<int32_t>();
		}

		//Devuelve un Vector4ui a partir de este.
		inline Vector4_t<uint32_t> ToVector4ui() const {
			return ToVec4<uint32_t>();
		}

		//Primera coordenada.
		//Posición X del rectángulo.
		T X;

		//Segunda coordenada.
		//Posición Y del rectángulo.
		T Y;

		//Tercera coordenada.
		//Ancho del rectángulo.
		T Z;

		//Cuarta coordenada.
		//Alto del rectángulo.
		T W;

	};


	//Representa un vector 4D.
	//Precisión = float.
	typedef Vector4_t<float_t> Vector4;

	
	//Representa un vector 4D.
	//Precisión = float.
	typedef Vector4_t<float_t> Vector4f;
	
	//Representa un vector 4D.
	//Precisión = double.
	typedef Vector4_t<double_t> Vector4d;
	
	//Representa un vector 4D.
	//Precisión = int32.
	typedef Vector4_t<int32_t> Vector4i;
	
	//Representa un vector 4D.
	//Precisión = uint32.
	typedef Vector4_t<uint32_t> Vector4ui;


}
