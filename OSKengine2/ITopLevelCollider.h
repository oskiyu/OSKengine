// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "OSKmacros.h"
#include "Vector3.hpp"
#include "IRayCollider.h"

namespace OSK::COLLISION {

	class Collider;

	class AxisAlignedBoundingBox;
	class SphereCollider;


	/// @brief Representa un área de colisión no muy ajustada
	/// que permite descartar colisiones de una manera más rápida.
	class OSKAPI_CALL ITopLevelCollider : public IRayCollider {

	public:

		virtual ~ITopLevelCollider() = default;

		OSK_DEFINE_AS(ITopLevelCollider);

		/// @brief Comprueba si este área de colisión está en contacto
		/// con otra.
		/// @param other Otra área de colisión de nivel alto.
		/// @param thisOffset Posición de este collider.
		/// @param otherOffset Posición del segundo collider.
		virtual bool IsColliding(const ITopLevelCollider& other,
			const Vector3f& thisOffset, const Vector3f& otherOffset) const = 0;

		virtual bool ContainsPoint(const Vector3f& thisOffset, const Vector3f& point) const = 0;


		/// @brief Calcula la intersección entre un rayo y el collider.
		/// @param ray Rayo.
		/// @param position Posición de este collider.
		/// @return Resultado de la intersección.
		virtual RayCastResult CastRay(const Ray& ray, const Vector3f& position) const = 0;

	protected:

		static bool AabbAabbCollision(const AxisAlignedBoundingBox& first, const AxisAlignedBoundingBox& other,
			const Vector3f& thisOffset, const Vector3f& otherOffset);

		static bool AabbSphereCollision(const AxisAlignedBoundingBox& first, const SphereCollider& other,
			const Vector3f& thisOffset, const Vector3f& otherOffset);

		static bool SphereSphereCollision(const SphereCollider& first, const SphereCollider& other, 
			const Vector3f& thisOffset, const Vector3f& otherOffset);

	};

}
