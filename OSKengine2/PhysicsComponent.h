#pragma once

#include "Component.h"
#include "Vector3.hpp"

namespace OSK::ECS {

	/// @brief Componente que permite simular las físicas de la entidad.
	class OSKAPI_CALL PhysicsComponent {

	public:

		OSK_COMPONENT("OSK::PhysicsComponent");

		/// @brief Establece el objeto como inmovible.
		/// @see SetMass(0.0f).
		void SetImmovable();

		/// @return True si el objeto tiene masa infinita.
		/// Inverse mass = 0.0f.
		bool IsImmovable() const;


		/// @brief Establece la masa del objeto.
		/// También actualiza la masa inversa.
		/// @param newMass Masa del objeto.
		/// @pre @p newMass > 0.
		/// @note Para una masa infinita, usar SetInverseMass(0).
		void SetMass(float newMass);

		/// @brief Establece la inversa de la masa del objeto.
		/// También actualiza la masa.
		/// Para masas infinitas, establecer a 0.
		/// @param newInverseMass Masa inversa.
		/// @pre @p newMass > 0.
		void SetInverseMass(float newInverseMass);


		/// @return Masa del objeto.
		float GetMass() const;

		/// @return 1 / masa del objeto.
		/// Para masas infinitas, devuelve 0.
		float GetInverseMass() const;


		/// @return Matriz de momento de inercia.
		glm::mat3 GetInertiaTensor() const;

		/// @return Matriz inversa del momento de inercia.
		glm::mat3 GetInverseInertiaTensor() const;


		void SetInertiaTensor(const glm::mat3& tensor);
		void SetInverseInertiaTensor(const glm::mat3& inverseTensor);


		/// @return Velocidad (en espacio del mundo).
		Vector3f GetVelocity() const;

		/// @return Cambio de velocidad en el frame actual.
		Vector3f GetCurrentFrameVelocityDelta() const;

		/// @param localSpacePoint Punto del objeto.
		/// @return Velocidad del punto (en espacio del mundo).
		Vector3f GetVelocityOfPoint(const Vector3f& localSpacePoint) const;

		/// @return Aceleración (m/s).
		/// En espacio del mundo.
		Vector3f GetAcceleration() const;


		/// @return Velocidad angular (rad/s).
		/// @todo check unidades.
		Vector3f GetAngularVelocity() const;


		/// @brief Quita todas las fuerzas aplicadas.
		void _ResetForces();

		/// @brief Reestablece el cambio de velocidad en el frame.
		void _ResetCurrentFrameDeltas();

		/// @brief Ejerce una fuerza sobre el centro de masa del objeto.
		/// @param force Fuerza (en Newtons: kg*m/s2).
		void ApplyForce(const Vector3f& force);

		/// @brief Ejerce un impulso sobre el punto del objeto.
		/// @param impulse Impulso (en Newtons: kg*m/s).
		void ApplyImpulse(const Vector3f& impulse, const Vector3f& localSpacePoint);


		/// @brief Establece la velocidad del objeto.
		/// @param velocity Nueva velocidad.
		void _SetVelocity(Vector3f velocity);

		/// @brief Establece la velocidad angular del objeto.
		/// @param angularVelocity Nueva velocidad angular.
		void _SetAngularVelocity(Vector3f angularVelocity);

		/// @brief Establece la acceleración del objeto.
		/// @param acceleration Nueva aceleración.
		void _SetAcceleration(Vector3f acceleration);


		/// @brief Centro de gravedad del objeto,
		/// en espacio local.
		Vector3f centerOfMassOffset = Vector3f::Zero;

		/// @brief Cantidad de fricción dinámica.
		float frictionCoefficient = 0.5f;

		/// @brief Coeficiente de resitución, que indica
		/// la cantidad de rebote del objeto.
		/// @pre @p coefficientOfRestitution >= 0.
		/// @pre @p coefficientOfRestitution <= 1.
		float coefficientOfRestitution = 0.2f;

		static Vector3f GetTorque(const Vector3f& localSpacePoint, const Vector3f& force);

	private:

		float mass = 1.0f;
		float inverseMass = 1.0f;

		glm::mat3 inertiaTensor = glm::mat3(1.5f);
		glm::mat3 inverseInertiaTensor = glm::inverse(glm::mat3(1.5f));


		// --- MOTION --- //

		/// @brief Velocidad de la entidad, en metros por segundo.
		Vector3f velocity = Vector3f::Zero;
		/// @brief Aceleración de la unidad, en metros por segundo al cuadrado (m/s^2).
		Vector3f acceleration = Vector3f::Zero;

		/// @brief Cambio de velocidad en el frame actual.
		Vector3f currentFrameVelocityDelta = Vector3f::Zero;

		/// @brief La dirección del vector indica el eje de rotación,
		/// y la longitud la cantidad de velocidad.
		Vector3f angularVelocity = Vector3f::Zero;

	};

}
