// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "OSKmacros.h"
#include "Vector3.hpp"
#include "IRayCollider.h"
#include "Transform3D.h"
#include "DetailedCollisionInfo.h"
#include "OwnedPtr.h"

#include "Gjk.h"
#include "Sat.h"

namespace OSK::COLLISION {

	class Collider;


	/// @brief Representa un �rea de colisi�n muy ajustada
	/// que permite detectar colisiones.
	class OSKAPI_CALL IBottomLevelCollider : public IGjkCollider, public ISatCollider {

	public:

		virtual ~IBottomLevelCollider() = default;

		OSK_DEFINE_AS(IBottomLevelCollider);

		virtual OwnedPtr<IBottomLevelCollider> CreateCopy() const = 0;


		/// @brief Comprueba si este �rea de colisi�n est� en contacto
		/// con otra, y devuelve la informaci�n detallada resultante.
		/// 
		/// @param other Otra �rea de colisi�n de nivel bajo.
		/// @param transformA Transform de este collider.
		/// @param transformB Transform del otro collider.
		/// 
		/// @return Informaci�n detallada de la colisi�n.
		virtual DetailedCollisionInfo GetCollisionInfo(const IBottomLevelCollider& other,
			const ECS::Transform3D& transformA, const ECS::Transform3D& transformB) const = 0;

		virtual bool ContainsPoint(const Vector3f& point) const = 0;

		/// @brief Transforma el collider.
		/// Debe llamarse una vez por frame.
		/// @param transform Transform de la entidad.
		virtual void Transform(const ECS::Transform3D& transform) = 0;

	};

}
