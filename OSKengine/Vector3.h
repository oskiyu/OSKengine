#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "Log.h"

#include <glm.hpp>

namespace OSK {

	//Representa un vector 3D.
	struct OSKAPI_CALL Vector3 {

		//Crea un vector 3D nulo { 0, 0, 0 }.
		Vector3();

		//Crea un vector 3D.
		Vector3(const float_t& x, const float_t& y, const float_t& z);

		//Crea una instancia del Vector2 en la que X, Y y Z tienen en mismo valor.
		//<value>: valor para X, Y y Z.
		Vector3(const float_t& value);

		//Crea un vector 3D con los parámetros de vec.
		Vector3(const glm::vec3& vec);

		//Operación Vector3 + Vector3.
		//X1 + X2; Y1 + Y2, Z1 + Z2.
		Vector3 operator+(const Vector3& vec) const;

		//Negación del Vector3.
		//-X; -Y; -Z.
		Vector3 operator-() const;

		//Operación Vector3 - Vector3.
		//X1 - X2; Y1 - Y2; Z1 - Z2.
		Vector3 operator-(const Vector3& vec) const;

		//Operación Vector3 * Vector3.
		//X1 * X2; Y1 * Y2; Z1 * Z2.
		Vector3 operator*(const Vector3& vec) const;

		//Operación Vector3 * float.
		//X * value; Y * value; Z * value.
		Vector3 operator*(const float_t& value) const;

		//Operación Vector3 / Vector3.
		//X / vec.X; Y / vec.Y, Z / vec.Z.
		Vector3 operator/(const Vector3& vec) const;

		//Operación Vector3 / float_t.
		//X / value; Y / value; Z / value.
		Vector3 operator/(const float_t& value) const;

		//Módulo del vector.
		//Obtiene la longitud del vector.
		float_t GetLenght() const;

		//Obtiene la distancia entre la posición representada por este vector y por el vector <vec>.
		float_t GetDistanceTo(const Vector3& vec) const;

		//Obtiene el producto escalar entre este vector y el vector <vec>.
		float_t Dot(const Vector3& vec) const;

		//Obtiene el producto vectorial entre este vector y el vector <vec>.
		Vector3 Cross(const Vector3& vec) const;

		//Devuelve un vector 3D normalizado con la misma dirección que este.
		Vector3 GetNormalized() const;

		//Normaliza el vector 3D para que su módulo (GetLength()) sea 1.
		void Normalize();

		//OSK::Vector2 a glm::vec2.
		glm::vec3 ToGLM() const;

		//Representa la primera coordenada.
		float_t X;

		//Representa la segunda coordenada.
		float_t Y;

		//Representa la tercera coordenada.
		float_t Z;

	};

}
