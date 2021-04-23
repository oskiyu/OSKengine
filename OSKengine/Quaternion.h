#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>

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

		/// <summary>
		/// Devuelve una matriz de rotaci�n.
		/// </summary>
		/// <returns>Matriz de rotaci�n.</returns>
		inline glm::mat4 ToMat4() const {
			return glm::toMat4(Quat);
		}

		/// <summary>
		/// Devuelve la rotaci�n del cuaterni�n como coordenadas de Euler.
		/// </summary>
		/// <returns>Rotaci�n.</returns>
		inline Vector3f ToEuler() const {
			return glm::eulerAngles(Quat);
		}

		//Crea un cuaterni�n a partir de las coordenadas de Euler.
		//	<rot>: rotaci�n.

		/// <summary>
		/// Crea un cuaterni�n a partir de las coordenadas de Euler.
		/// </summary>
		/// <param name="rot">Rotaci�n.</param>
		/// <returns>Quaterni�n.</returns>
		static Quaternion CreateFromEulerAngles(const Vector3f& rot);

		/// <summary>
		/// Interpola este cuaternion con otro.
		/// </summary>
		/// <param name="other">Otro cuaternion.</param>
		/// <param name="factor">Factor de interpolaci�n.</param>
		/// <returns>Quaternion interpolado.</returns>
		Quaternion Interpolate(const Quaternion& other, float factor) const;

		/// <summary>
		/// Cuaterni�n.
		/// </summary>
		glm::quat Quat;

	};

}
