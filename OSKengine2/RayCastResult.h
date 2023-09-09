// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include <limits>
#include "Vector3.hpp"

namespace OSK::COLLISION {

	/// @brief Representa el resultado de trazar un rayo en la escena.
	/// El resultado puede ser positivo en caso de que el rayo haya interseccionado
	/// con algun objeto, o negativo en caso contrario.
	class RayCastResult {

	public:

		/// @brief Crea un resultado falso (sin intersección).
		/// @return Resultado negativo.
		static RayCastResult False();

		/// @brief Crea un resultado verdadero (con intersección).
		/// @param intersection Punto de intersección más cercano.
		/// @return Resultado positivo.
		static RayCastResult True(const Vector3f& intersection);

		/// @brief Devuelve true si hubo intersección, o false en caso contrario.
		bool Result() const;

		/// @brief Devuelve el punto de intersección más cercano al origen.
		/// @return Punto de intersección.
		const Vector3f& GetIntersectionPoint() const;

	private:

		RayCastResult(bool intersection, const Vector3f& point);

		Vector3f intersectionPoint;
		bool intersection = false;

	};

}
