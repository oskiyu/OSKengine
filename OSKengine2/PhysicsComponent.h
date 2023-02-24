#pragma once

#include "Component.h"
#include "Vector3.hpp"

namespace OSK::ECS {

	/// @brief Componente que permite simular las físicas de la entidad.
	class OSKAPI_CALL PhysicsComponent {

	public:

		OSK_COMPONENT("OSK::PhysicsComponent");

		/// @brief Velocidad de la entidad, en metros por segundo.
		Vector3f velocity = 0.0f;
		/// @brief Aceleración de la unidad, en metros por segundo al cuadrado (m/s^2).
		Vector3f acceleration = 0.0f;

		/// @brief Peso de la entidad, en kilogramos.
		float mass = 0.0f;


		/// @brief Ejerce una fuerza sobre el centro de masa del objeto.
		/// @param force Fuerza (en Newtons: kg*m/s2).
		void ApplyForce(const Vector3f& force);

		/// @brief Aplica una fuerza de manera instantánea (en un tiempo
		/// infinitesimal).
		/// @param impulse Fuerza.
		void ApplyImpulse(const Vector3f& impulse);


		/// @brief Obtiene el momento (velocidad * masa) del centro de masa del objeto.
		/// @return Momento del objeto.
		Vector3f GetMomentum() const;

	};

}
