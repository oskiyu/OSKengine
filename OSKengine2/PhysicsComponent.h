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
		float weight = 0.0f;

	};

}
