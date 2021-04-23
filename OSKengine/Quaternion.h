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

		/// <summary>
		/// Devuelve una matriz de rotación.
		/// </summary>
		/// <returns>Matriz de rotación.</returns>
		inline glm::mat4 ToMat4() const {
			return glm::toMat4(Quat);
		}

		/// <summary>
		/// Devuelve la rotación del cuaternión como coordenadas de Euler.
		/// </summary>
		/// <returns>Rotación.</returns>
		inline Vector3f ToEuler() const {
			return glm::eulerAngles(Quat);
		}

		//Crea un cuaternión a partir de las coordenadas de Euler.
		//	<rot>: rotación.

		/// <summary>
		/// Crea un cuaternión a partir de las coordenadas de Euler.
		/// </summary>
		/// <param name="rot">Rotación.</param>
		/// <returns>Quaternión.</returns>
		static Quaternion CreateFromEulerAngles(const Vector3f& rot);

		/// <summary>
		/// Interpola este cuaternion con otro.
		/// </summary>
		/// <param name="other">Otro cuaternion.</param>
		/// <param name="factor">Factor de interpolación.</param>
		/// <returns>Quaternion interpolado.</returns>
		Quaternion Interpolate(const Quaternion& other, float factor) const;

		/// <summary>
		/// Cuaternión.
		/// </summary>
		glm::quat Quat;

	};

}
