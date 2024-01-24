// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include <limits>
#include "Vector3.hpp"
#include "GameObject.h"

namespace OSK::COLLISION {

	/// @brief Representa el resultado de trazar un rayo en la escena.
	/// El resultado puede ser positivo en caso de que el rayo haya interseccionado
	/// con algun objeto, o negativo en caso contrario.
	class OSKAPI_CALL RayCastResult {

	public:

		/// @brief Crea un resultado falso (sin intersección).
		/// @return Resultado negativo.
		static RayCastResult False();

		/// @brief Crea un resultado verdadero (con intersección).
		/// @param intersection Punto de intersección más cercano.
		/// @param gameObject Objeto interseccionado.
		/// @return Resultado positivo.
		static RayCastResult True(
			const Vector3f& intersection, 
			ECS::GameObjectIndex gameObject);

		/// @brief Devuelve true si hubo intersección, o false en caso contrario.
		bool Result() const;

		/// @brief Devuelve el punto de intersección más cercano al origen.
		/// @return Punto de intersección.
		const Vector3f& GetIntersectionPoint() const;

		/// @return Objeto con el que intersecciona.
		ECS::GameObjectIndex GetObject() const;

	private:

		RayCastResult(bool intersection, const Vector3f& point, ECS::GameObjectIndex gameObject);

		Vector3f m_intersectionPoint = Vector3f::Zero;
		bool m_intersection = false;
		ECS::GameObjectIndex m_object = ECS::EMPTY_GAME_OBJECT;

	};

}
