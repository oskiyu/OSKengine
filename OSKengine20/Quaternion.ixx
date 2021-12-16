#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>

export module OSKengine.Types.Quaternion;

export import OSKengine.Types.Vector3;

export namespace OSK {

	/// <summary>
	/// Representa un cuaterni�n, para representar una orientaci�n.
	/// </summary>
	class Quaternion {

	public:

		/// <summary>
		/// Crea el cuaterni�n.
		/// </summary>
		Quaternion() {
			quat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
			glm::normalize(quat);
		}

		/// <summary>
		/// Rota el cuaterni�n dado un eje, que se interpreta en espacio del mundo.
		/// </summary>
		/// <param name="angle">�ngulo que se va a rotar.</param>
		/// <param name="axis">Eje, en coordenadas del mundo, sobre el que se va a rotar.</param>
		void Rotate_WorldSpace(float angle, const Vector3f& axis) {
			glm::quat rot = glm::normalize(glm::angleAxis(angle, axis.ToGLM()));

			quat = quat * (glm::inverse(quat) * rot) * quat;
		}

		/// <summary>
		/// Rota el cuaterni�n dado un eje, que se interpreta en espacio local.
		/// </summary>
		/// <param name="angle">�ngulo que se va a rotar.</param>
		/// <param name="axis">Eje, en coordenadas del cuaterni�n, sobre el que se va a rotar.</param>
		void Rotate_LocalSpace(float angle, const Vector3f& axis) {
			quat = quat * glm::normalize(glm::angleAxis(angle, axis.ToGLM()));
		}

		/// <summary>
		/// Devuelve una matriz de rotaci�n.
		/// </summary>
		/// <returns>Matriz de rotaci�n.</returns>
		inline glm::mat4 ToMat4() const {
			return glm::toMat4(quat);
		}

		/// <summary>
		/// Devuelve la rotaci�n del cuaterni�n como coordenadas de Euler.
		/// </summary>
		/// <returns>Rotaci�n.</returns>
		inline Vector3f ToEuler() const {
			return glm::eulerAngles(quat);
		}

		//Crea un cuaterni�n a partir de las coordenadas de Euler.
		//	<rot>: rotaci�n.

		/// <summary>
		/// Crea un cuaterni�n a partir de las coordenadas de Euler.
		/// </summary>
		/// <param name="rot">Rotaci�n.</param>
		/// <returns>Quaterni�n.</returns>
		static Quaternion CreateFromEulerAngles(const Vector3f& rot) {
			Quaternion q;
			q.quat = glm::quat(rot.ToGLM());

			return q;
		}

		/// <summary>
		/// Interpola este cuaternion con otro.
		/// </summary>
		/// <param name="other">Otro cuaternion.</param>
		/// <param name="factor">Factor de interpolaci�n.</param>
		/// <returns>Quaternion interpolado.</returns>
		Quaternion Interpolate(const Quaternion& other, float factor) const {   // calc cosine theta
			float cosom = quat.x * other.quat.x + quat.y * other.quat.y + quat.z * other.quat.z + quat.w * other.quat.w;

			// adjust signs (if necessary)
			Quaternion end = other;
			if (cosom < static_cast<float>(0.0))
			{
				cosom = -cosom;
				end.quat.x = -end.quat.x;   // Reverse all signs
				end.quat.y = -end.quat.y;
				end.quat.z = -end.quat.z;
				end.quat.w = -end.quat.w;
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
			output.quat.x = sclp * quat.x + sclq * end.quat.x;
			output.quat.y = sclp * quat.y + sclq * end.quat.y;
			output.quat.z = sclp * quat.z + sclq * end.quat.z;
			output.quat.w = sclp * quat.w + sclq * end.quat.w;

			return output;
		}

		glm::quat& GetGlm() {
			return quat;
		}
		glm::quat GetGlm() const {
			return quat;
		}

	private:

		/// <summary>
		/// Cuaterni�n.
		/// </summary>
		glm::quat quat;

	};

}