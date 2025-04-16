// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "ApiCall.h"
#include "DefineAs.h"

#include "Vector3.hpp"
#include "IRayCollider.h"

#include "Plane.h"
#include "Frustum.h"

#include "UniquePtr.hpp"

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

		virtual UniquePtr<ITopLevelCollider> CreateCopy() const = 0;

		/// @brief Comprueba si este área de colisión está en contacto
		/// con otra.
		/// @param other Otra área de colisión de nivel alto.
		/// @param thisOffset Posición de este collider.
		/// @param otherOffset Posición del segundo collider.
		virtual bool IsColliding(const ITopLevelCollider& other) const = 0;

		virtual bool ContainsPoint(const Vector3f& point) const = 0;


		/// @brief Comprueba si el collider está COMPLETMENTE por detrás del plano.
		/// @param plane Plano a comprobar.
		/// @return True si está completamente por detrás.
		virtual bool IsBehindPlane(Plane plane) const = 0;

		/// @return True si el collider está dentro del frustum indicado.
		bool IsInsideFrustum(const AnyFrustum& frustum) const;


		/// @brief Calcula la intersección entre un rayo y el collider.
		/// @param ray Rayo.
		/// @param position Posición de este collider.
		/// @return Resultado de la intersección.
		virtual RayCastResult CastRay(const Ray& ray) const = 0;


		void SetPosition(const Vector3f& position);
		const Vector3f& GetPosition() const;

	protected:

		static bool AabbAabbCollision(const AxisAlignedBoundingBox& first, const AxisAlignedBoundingBox& other);

		static bool AabbSphereCollision(const AxisAlignedBoundingBox& first, const SphereCollider& other);

		static bool SphereSphereCollision(const SphereCollider& first, const SphereCollider& other);

	private:

		Vector3f m_position = Vector3f::Zero;

	};

}
