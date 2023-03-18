#pragma once

#include "OSKmacros.h"
#include "Vector3.hpp"

#include <glm/gtc/quaternion.hpp>

namespace OSK {

	/// <summary>
	/// Representa un cuaterni�n, para representar una orientaci�n.
	/// </summary>
	class OSKAPI_CALL Quaternion {

	public:

		/// <summary>
		/// Crea el cuaterni�n.
		/// </summary>
		Quaternion();

		/// <summary>
		/// Rota el cuaterni�n dado un eje, que se interpreta en espacio del mundo.
		/// </summary>
		/// <param name="angle">�ngulo que se va a rotar.</param>
		/// <param name="axis">Eje, en coordenadas del mundo, sobre el que se va a rotar.</param>
		void Rotate_WorldSpace(float angle, const Vector3f& axis);

		/// <summary>
		/// Rota el cuaterni�n dado un eje, que se interpreta en espacio local.
		/// </summary>
		/// <param name="angle">�ngulo que se va a rotar.</param>
		/// <param name="axis">Eje, en coordenadas del cuaterni�n, sobre el que se va a rotar.</param>
		void Rotate_LocalSpace(float angle, const Vector3f& axis);

		/// @brief Rota un vector con la orientaci�n de este quaterni�n.
		/// @param vec Vector a rotar.
		/// @return Vector rotado.
		Vector3f RotateVector(const Vector3f& vec) const;

		/// <summary>
		/// Devuelve la rotaci�n del cuaterni�n como coordenadas de Euler.
		/// </summary>
		/// <returns>Rotaci�n.</returns>
		Vector3f ToEulerAngles() const;

		/// <summary>
		/// Crea un cuaterni�n a partir de las coordenadas de Euler.
		/// </summary>
		/// <param name="rot">Rotaci�n.</param>
		/// <returns>Quaterni�n.</returns>
		static Quaternion CreateFromEulerAngles(const Vector3f& rot);

		/// <summary>
		/// Crea un cuaterni�n a partir del dado.
		/// </summary>
		static Quaternion FromGlm(const glm::quat& quat);

		/// <summary>
		/// Interpola este cuaternion con otro.
		/// </summary>
		/// <param name="other">Otro cuaternion.</param>
		/// <param name="factor">Factor de interpolaci�n.</param>
		/// <returns>Quaternion interpolado.</returns>
		Quaternion Interpolate(const Quaternion& other, float factor) const;

		/// <summary>
		/// Devuelve el cuaterni�n en formato GLM.
		/// </summary>
		glm::quat ToGlm() const;

		/// <summary>
		/// Devuelve una matriz de rotaci�n.
		/// </summary>
		/// <returns>Matriz de rotaci�n.</returns>
		glm::mat4 ToMat4() const;

	private:

		/// <summary>
		/// Cuaterni�n.
		/// </summary>
		glm::quat quaternion;

	};

}
