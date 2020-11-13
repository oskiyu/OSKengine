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

	//Representa un cuaternión, para representar una orientación.
	class OSKAPI_CALL Quaternion {

	public:

		//Crea el cuaternión.
		Quaternion();

		//Rota el cuaternión dado un eje, que se interpreta en espacio del mundo.
		//	<angle>: ángulo que se va a rotar.
		//	<axis>: eje, en coordenadas del mundo, sobre el que se va a rotar.
		void Rotate_WorldSpace(float angle, const Vector3f& axis);

		//Rota el cuaternión dado un eje, que se interpreta en espacio local.
		//	<angle>: ángulo que se va a rotar.
		//	<axis>: eje, en coordenadas del cuaternión, sobre el que se va a rotar.
		void Rotate_LocalSpace(float angle, const Vector3f& axis);

		//Devuelve una matriz de rotación.
		inline glm::mat4 ToMat4() const {
			return glm::toMat4(Quat);
		}

		//Devuelve la rotación del cuaternión como coordenadas de Euler.
		inline Vector3f ToEuler() const {
			return glm::eulerAngles(Quat);
		}

		//Crea un cuaternión a partir de las coordenadas de Euler.
		//	<rot>: rotación.
		static Quaternion CreateFromEulerAngles(const Vector3f& rot);

		//Cuaternión.
		glm::quat Quat;

	};

}
