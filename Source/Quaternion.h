#pragma once

#include "OSKmacros.h"
#include "Vector3.hpp"

#include <glm/gtc/quaternion.hpp>

namespace OSK {

	/// @brief Representa un cuaterni�n, para representar una orientaci�n.
	class OSKAPI_CALL Quaternion {

	public:

		/// @brief Crea un cuaterni�n vac�o.
		Quaternion() = default;


		/// @brief Crea un cuaterni�n vac�o.
		static Quaternion Empty();

		/// @brief Crea un cuaterni�n a partir de las coordenadas de Euler.
		/// @param rot Rotaci�n (Euler).
		static Quaternion CreateFromEulerAngles(const Vector3f& rot);

		/// @brief Crea un cuaterni�n a partir del dado.
		/// @param quat Cuaterni�n original.
		static Quaternion FromGlm(const glm::quat& quat);


		/// @brief Rota el cuaterni�n dado un eje, que se interpreta en espacio del mundo.
		/// @param angle �ngulo que se va a rotar.
		/// @param axis Eje, en coordenadas del mundo, sobre el que se va a rotar.
		void Rotate_WorldSpace(float angle, const Vector3f& axis);

		/// @brief Rota el cuaterni�n dado un eje, que se interpreta en espacio local.
		/// @param angle �ngulo que se va a rotar.
		/// @param axis Eje, en coordenadas del cuaterni�n, sobre el que se va a rotar.
		void Rotate_LocalSpace(float angle, const Vector3f& axis);


		/// @brief Aplica una rotaci�n en un nuevo quaternion.
		/// @param other Rotaci�n a aplicar.
		/// @return Nuevo cuaternion, resultado de aplicar @p other
		/// sobre este cuaterni�n.
		/// @note Este cuaterni�n no ser� modificado.
		Quaternion operator+(const Quaternion& other) const;

		/// @brief Aplica una rotaci�n al quaternion.
		/// @param other Rotaci�n a aplicar.
		/// @return Self.
		Quaternion& operator+=(const Quaternion& other);


		/// @param other Otro cuaterni�n.
		/// @return Diferencia entre este cuaternion y el otro (this - other).
		/// @note Este cuaterni�n no ser� modificado.
		Quaternion operator-(const Quaternion& other) const;

		/// @brief Quita una rotaci�n del cuaterni�n.
		/// @param other Otro cuaterni�n.
		/// @return Self. 
		Quaternion& operator-=(const Quaternion& other);


		/// @brief Rota un vector con la orientaci�n de este quaterni�n.
		/// @param vec Vector a rotar.
		/// @return Vector rotado.
		Vector3f RotateVector(const Vector3f& vec) const;


		/// @return Rotaci�n del cuaterni�n como coordenadas de Euler.
		Vector3f ToEulerAngles() const;


		/// @brief Interpola este cuaternion con otro.
		/// @param other Otro cuaternion.
		/// @param factor Factor de interpolaci�n.
		/// @return Quaternion interpolado.
		Quaternion Interpolate(const Quaternion& other, float factor) const;


		/// @return Cuaterni�n en formato GLM.
		glm::quat ToGlm() const;

		/// @return Matriz de rotaci�n.
		glm::mat4 ToMat4() const;

	private:

		glm::quat m_quaternion = glm::identity<glm::quat>();

	};

}
