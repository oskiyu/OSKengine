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

	//Representa un cuaterni�n, para representar una orientaci�n.
	class OSKAPI_CALL Quaternion {

	public:

		//Crea el cuaterni�n.
		Quaternion();

		//Rota el cuaterni�n dado un eje, que se interpreta en espacio del mundo.
		//	<angle>: �ngulo que se va a rotar.
		//	<axis>: eje, en coordenadas del mundo, sobre el que se va a rotar.
		void Rotate_WorldSpace(float angle, const Vector3f& axis);

		//Rota el cuaterni�n dado un eje, que se interpreta en espacio local.
		//	<angle>: �ngulo que se va a rotar.
		//	<axis>: eje, en coordenadas del cuaterni�n, sobre el que se va a rotar.
		void Rotate_LocalSpace(float angle, const Vector3f& axis);

		//Devuelve una matriz de rotaci�n.
		inline glm::mat4 ToMat4() const {
			return glm::toMat4(Quat);
		}

		//Devuelve la rotaci�n del cuaterni�n como coordenadas de Euler.
		inline Vector3f ToEuler() const {
			return glm::eulerAngles(Quat);
		}

		//Crea un cuaterni�n a partir de las coordenadas de Euler.
		//	<rot>: rotaci�n.
		static Quaternion CreateFromEulerAngles(const Vector3f& rot);

		//Cuaterni�n.
		glm::quat Quat;

	};

}
