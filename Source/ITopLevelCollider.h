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


	/// @brief Representa un �rea de colisi�n no muy ajustada
	/// que permite descartar colisiones de una manera m�s r�pida.
	class OSKAPI_CALL ITopLevelCollider : public IRayCollider {

	public:

		virtual ~ITopLevelCollider() = default;

		OSK_DEFINE_AS(ITopLevelCollider);

		virtual UniquePtr<ITopLevelCollider> CreateCopy() const = 0;

		/// @brief Comprueba si este �rea de colisi�n est� en contacto
		/// con otra.
		/// @param other Otra �rea de colisi�n de nivel alto.
		/// @param thisOffset Posici�n de este collider.
		/// @param otherOffset Posici�n del segundo collider.
		virtual bool IsColliding(const ITopLevelCollider& other) const = 0;

		virtual bool ContainsPoint(const Vector3f& point) const = 0;


		/// @brief Comprueba si el collider est� COMPLETMENTE por detr�s del plano.
		/// @param plane Plano a comprobar.
		/// @return True si est� completamente por detr�s.
		virtual bool IsBehindPlane(Plane plane) const = 0;

		/// @return True si el collider est� dentro del frustum indicado.
		bool IsInsideFrustum(const AnyFrustum& frustum) const;


		/// @brief Calcula la intersecci�n entre un rayo y el collider.
		/// @param ray Rayo.
		/// @param position Posici�n de este collider.
		/// @return Resultado de la intersecci�n.
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
