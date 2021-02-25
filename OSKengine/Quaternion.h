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

        Quaternion Interpolate(const Quaternion& other, float factor) {   // calc cosine theta
            float cosom = Quat.x * other.Quat.x + Quat.y * other.Quat.y + Quat.z * other.Quat.z + Quat.w * other.Quat.w;

            // adjust signs (if necessary)
            Quaternion end = other;
            if (cosom < static_cast<float>(0.0))
            {
                cosom = -cosom;
                end.Quat.x = -end.Quat.x;   // Reverse all signs
                end.Quat.y = -end.Quat.y;
                end.Quat.z = -end.Quat.z;
                end.Quat.w = -end.Quat.w;
            }

            // Calculate coefficients
            float sclp, sclq;
            if ((static_cast<float>(1.0) - cosom) > static_cast<float>(0.0001)) // 0.0001 -> some epsillon
            {
                // Standard case (slerp)
                float omega, sinom;
                omega = std::acos(cosom); // extract theta from dot product's cos theta
                sinom = std::sin(omega);
                sclp = std::sin((static_cast<float>(1.0) - factor) * omega) / sinom;
                sclq = std::sin(factor * omega) / sinom;
            }
            else
            {
                // Very close, do linear interp (because it's faster)
                sclp = static_cast<float>(1.0) - factor;
                sclq = factor;
            }

            Quaternion output;
            output.Quat.x = sclp * Quat.x + sclq * end.Quat.x;
            output.Quat.y = sclp * Quat.y + sclq * end.Quat.y;
            output.Quat.z = sclp * Quat.z + sclq * end.Quat.z;
            output.Quat.w = sclp * Quat.w + sclq * end.Quat.w;

            return output;
        }

		//Cuaternión.
		glm::quat Quat;

	};

}
