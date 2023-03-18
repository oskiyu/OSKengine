#pragma once

#include "OSKmacros.h"
#include "Vector3.hpp"

#include <glm/gtc/quaternion.hpp>

namespace OSK {

	/// <summary>
	/// Representa un cuaternión, para representar una orientación.
	/// </summary>
	class OSKAPI_CALL Quaternion {

	public:

		/// <summary>
		/// Crea el cuaternión.
		/// </summary>
		Quaternion();

		/// <summary>
		/// Rota el cuaternión dado un eje, que se interpreta en espacio del mundo.
		/// </summary>
		/// <param name="angle">Ángulo que se va a rotar.</param>
		/// <param name="axis">Eje, en coordenadas del mundo, sobre el que se va a rotar.</param>
		void Rotate_WorldSpace(float angle, const Vector3f& axis);

		/// <summary>
		/// Rota el cuaternión dado un eje, que se interpreta en espacio local.
		/// </summary>
		/// <param name="angle">Ángulo que se va a rotar.</param>
		/// <param name="axis">Eje, en coordenadas del cuaternión, sobre el que se va a rotar.</param>
		void Rotate_LocalSpace(float angle, const Vector3f& axis);

		/// @brief Rota un vector con la orientación de este quaternión.
		/// @param vec Vector a rotar.
		/// @return Vector rotado.
		Vector3f RotateVector(const Vector3f& vec) const;

		/// <summary>
		/// Devuelve la rotación del cuaternión como coordenadas de Euler.
		/// </summary>
		/// <returns>Rotación.</returns>
		Vector3f ToEulerAngles() const;

		/// <summary>
		/// Crea un cuaternión a partir de las coordenadas de Euler.
		/// </summary>
		/// <param name="rot">Rotación.</param>
		/// <returns>Quaternión.</returns>
		static Quaternion CreateFromEulerAngles(const Vector3f& rot);

		/// <summary>
		/// Crea un cuaternión a partir del dado.
		/// </summary>
		static Quaternion FromGlm(const glm::quat& quat);

		/// <summary>
		/// Interpola este cuaternion con otro.
		/// </summary>
		/// <param name="other">Otro cuaternion.</param>
		/// <param name="factor">Factor de interpolación.</param>
		/// <returns>Quaternion interpolado.</returns>
		Quaternion Interpolate(const Quaternion& other, float factor) const;

		/// <summary>
		/// Devuelve el cuaternión en formato GLM.
		/// </summary>
		glm::quat ToGlm() const;

		/// <summary>
		/// Devuelve una matriz de rotación.
		/// </summary>
		/// <returns>Matriz de rotación.</returns>
		glm::mat4 ToMat4() const;

	private:

		/// <summary>
		/// Cuaternión.
		/// </summary>
		glm::quat quaternion;

	};

}
