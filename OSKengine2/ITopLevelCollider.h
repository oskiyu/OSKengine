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


	/// @brief Representa un �rea de colisi�n no muy ajustada
	/// que permite descartar colisiones de una manera m�s r�pida.
	class OSKAPI_CALL ITopLevelCollider : public IRayCollider {

	public:

		virtual ~ITopLevelCollider() = default;

		OSK_DEFINE_AS(ITopLevelCollider);

		/// @brief Comprueba si este �rea de colisi�n est� en contacto
		/// con otra.
		/// @param other Otra �rea de colisi�n de nivel alto.
		/// @param thisOffset Posici�n de este collider.
		/// @param otherOffset Posici�n del segundo collider.
		virtual bool IsColliding(const ITopLevelCollider& other,
			const Vector3f& thisOffset, const Vector3f& otherOffset) const = 0;

		virtual bool ContainsPoint(const Vector3f& thisOffset, const Vector3f& point) const = 0;


		/// @brief Calcula la intersecci�n entre un rayo y el collider.
		/// @param ray Rayo.
		/// @param position Posici�n de este collider.
		/// @return Resultado de la intersecci�n.
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
