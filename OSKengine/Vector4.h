#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "Log.h"

#include <glm.hpp>

#include "Vector2.h"

namespace OSK {

	//Representa un vector con 4 coordenadas (Rectangle).
	struct OSKAPI_CALL Vector4 {

		//Crea un vector 4D nulo { 0, 0, 0, 0 }.
		Vector4();

		//Crea un vector 4D.
		Vector4(const float_t& x, const float_t& y, const float_t& z, const float_t& w);

		//Crea una instancia del Vector2 en la que X, Y, Z y W tienen en mismo valor.
		//<value>: valor para X, Y, Z y W.
		Vector4(const float_t& value);

		//Crea un vector 4D con los parámetros de vec.
		Vector4(const glm::vec4& vec);

		//Operación Vector4 + Vector4.
		//X1 + X2; Y1 + Y2, Z1 + Z2, W1 + W2.
		Vector4 operator+(const Vector4& vec) const;

		//Negación del Vector4.
		//-X; -Y; -Z; -W.
		Vector4 operator-() const;

		//Operación Vector4 - Vector4.
		//X1 - X2; Y1 - Y2; Z1 - Z2, W1 - W2.
		Vector4 operator-(const Vector4& vec) const;

		//Operación Vector4 * Vector4.
		//X1 * X2; Y1 * Y2; Z1 * Z2; W1 * W2.
		Vector4 operator*(const Vector4& vec) const;

		//Operación Vector4 * float.
		//X * value; Y * value; Z * value; W * value.
		Vector4 operator*(const float_t& value) const;

		//Operación Vector4 / Vector4.
		//X / vec.X; Y / vec.Y, Z / vec.Z, W / vec.W.
		Vector4 operator/(const Vector4& vec) const;

		//Operación Vector4 / float_t.
		//X / value; Y / value; Z / value; W / value.
		Vector4 operator/(const float_t& value) const;

		//Obtiene la posición del rectángulo (X, Y).
		Vector2 GetRectanglePosition() const;

		//Obtiene el tamaño del rectángulo (Z, W).
		Vector2 GetRectangleSize() const;

		//Obtiene la anchura del rectángulo (Z).
		float_t GetRectangleWidth() const;

		//Obtiene la altura del rectángulo (W).
		float_t GetRectangleHeight() const;

		//Obtiene el tope del rectángulo (Y + W).
		float_t GetRectangleTop() const;

		//Obtiene el lateral derecho del tectángulo (X + Z).
		float_t GetRectangleRight() const;

		Vector2 GetRectangleMiddlePoint() const;

		//Retorna 'true' si los dos rectángulos se tocan.
		bool Intersects(const Vector4& vec) const;

		//OSK::Vector4 a glm::vec4.
		glm::vec4 ToGLM() const;

		//Primera coordenada.
		//Posición X del rectángulo.
		float_t X;

		//Segunda coordenada.
		//Posición Y del rectángulo.
		float_t Y;

		//Tercera coordenada.
		//Ancho del rectángulo.
		float_t Z;

		//Cuarta coordenada.
		//Alto del rectángulo.
		float_t W;

	};

}
