// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "OSKmacros.h"
#include "RayCastResult.h"
#include "Ray.h"

namespace OSK::COLLISION {

	/// @brief Interfaz para colliders que pueden interseccionar
	/// con un rayo.
	class OSKAPI_CALL IRayCollider {

	public:

		virtual RayCastResult CastRay(const Ray& ray) const = 0;

	};

}
