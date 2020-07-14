#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "Log.h"

#include <glm.hpp>

namespace OSK {

	//Representa un vector 2D.
	struct OSKAPI_CALL Vector2 {

		//Crea un vector 2D nulo { 0, 0 }.
		Vector2();

		//Crea una instancia del Vector2.
		Vector2(const float_t& x, const float_t& y);

		//Crea una instancia del Vector2 en la que X e Y tienen en mismo valor.
		//<value>: valor para X e Y.
		Vector2(const float_t& value);

		//Crea un vector 2D con los parámetros de vec.
		Vector2(const glm::vec2& vec);

		//Operación Vector2 + Vector2.
		//X1 + X2; Y1 + Y2.
		Vector2 operator+(const Vector2& vec) const;

		//Negación del Vector2.
		//-X; -Y.
		Vector2 operator-() const;

		//Operación Vector2 - Vector2.
		//X1 - X2; Y1 - Y2.
		Vector2 operator-(const Vector2& vec) const;

		//Operación Vector2 * Vector2.
		//X1 * X2; Y1 * Y2.
		Vector2 operator*(const Vector2& vec) const;

		//Operación Vector2 * float_t.
		//X * value; Y * value.
		Vector2 operator*(const float_t& value) const;

		//Operación Vector2 / Vector2.
		//X = vec.X; Y = vec.Y.
		Vector2 operator/(const Vector2& value) const;

		//Operación Vector2 / float_t.
		//X = value; Y = value.
		Vector2 operator/(const float_t& value) const;

		//Módulo del vector.
		//Obtiene la longitud del vector.
		float_t GetLength() const;

		//Obtiene la distancia entre la posición representada por este vector y por el vector <vec>.
		float_t GetDistanceTo(const Vector2& vec) const;

		//Producto escalar de dos vectores.
		float_t Dot(const Vector2& vec) const;

		//Devuelve un vector 2D normalizado con la misma dirección que este.
		Vector2 GetNormalized() const;

		//Normaliza el vector 2D para que su módulo (GetLength()) sea 1.
		void Normalize();

		//OSK::Vector2 a glm::vec2.
		glm::vec2 ToGLM() const;

		//Representa la primera coordenada.
		float_t X;

		//Representa la segunda coordenada.
		float_t Y;

	};

}
