// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "ApiCall.h"
#include "RayCastResult.h"
#include "Ray.h"

namespace OSK::COLLISION {

	/// @brief Interfaz para colliders que pueden interseccionar
	/// con un rayo.
	class OSKAPI_CALL IRayCollider {

	public:

		virtual ~IRayCollider() = default;

		/// @brief Calcula la intersecci�n entre un rayo y el collider.
		/// @paramm ray Rayo.
		/// @param position Posici�n de este collider.
		/// @return Resultado de la intersecci�n.
		virtual RayCastResult CastRay(const Ray& ray) const = 0;

	};

}
