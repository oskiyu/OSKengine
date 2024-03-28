#pragma once

#include "OSKmacros.h"
#include "Vector3.hpp"

#include <glm/gtc/quaternion.hpp>

namespace OSK {

	/// @brief Representa un cuaternión, para representar una orientación.
	class OSKAPI_CALL Quaternion {

	public:

		/// @brief Crea un cuaternión vacío.
		Quaternion() = default;


		/// @brief Crea un cuaternión vacío.
		static Quaternion Empty();

		/// @brief Crea un cuaternión a partir de las coordenadas de Euler.
		/// @param rot Rotación (Euler).
		static Quaternion CreateFromEulerAngles(const Vector3f& rot);

		/// @brief Crea un cuaternión a partir del dado.
		/// @param quat Cuaternión original.
		static Quaternion FromGlm(const glm::quat& quat);


		/// @brief Rota el cuaternión dado un eje, que se interpreta en espacio del mundo.
		/// @param angle Ángulo que se va a rotar.
		/// @param axis Eje, en coordenadas del mundo, sobre el que se va a rotar.
		void Rotate_WorldSpace(float angle, const Vector3f& axis);

		/// @brief Rota el cuaternión dado un eje, que se interpreta en espacio local.
		/// @param angle Ángulo que se va a rotar.
		/// @param axis Eje, en coordenadas del cuaternión, sobre el que se va a rotar.
		void Rotate_LocalSpace(float angle, const Vector3f& axis);


		/// @brief Aplica una rotación en un nuevo quaternion.
		/// @param other Rotación a aplicar.
		/// @return Nuevo cuaternion, resultado de aplicar @p other
		/// sobre este cuaternión.
		/// @note Este cuaternión no será modificado.
		Quaternion operator+(const Quaternion& other) const;

		/// @brief Aplica una rotación al quaternion.
		/// @param other Rotación a aplicar.
		/// @return Self.
		Quaternion& operator+=(const Quaternion& other);


		/// @param other Otro cuaternión.
		/// @return Diferencia entre este cuaternion y el otro (this - other).
		/// @note Este cuaternión no será modificado.
		Quaternion operator-(const Quaternion& other) const;

		/// @brief Quita una rotación del cuaternión.
		/// @param other Otro cuaternión.
		/// @return Self. 
		Quaternion& operator-=(const Quaternion& other);


		/// @brief Rota un vector con la orientación de este quaternión.
		/// @param vec Vector a rotar.
		/// @return Vector rotado.
		Vector3f RotateVector(const Vector3f& vec) const;


		/// @return Rotación del cuaternión como coordenadas de Euler.
		Vector3f ToEulerAngles() const;


		/// @brief Interpola este cuaternion con otro.
		/// @param other Otro cuaternion.
		/// @param factor Factor de interpolación.
		/// @return Quaternion interpolado.
		Quaternion Interpolate(const Quaternion& other, float factor) const;


		/// @return Cuaternión en formato GLM.
		glm::quat ToGlm() const;

		/// @return Matriz de rotación.
		glm::mat4 ToMat4() const;

	private:

		glm::quat m_quaternion = glm::identity<glm::quat>();

	};

}
