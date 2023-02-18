// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "OSKmacros.h"
#include "Vector3.hpp"
#include "IRayCollider.h"
#include "Transform3D.h"
#include "DetailedCollisionInfo.h"

namespace OSK::COLLISION {

	class Collider;


	/// @brief Representa un área de colisión muy ajustada
	/// que permite detectar colisiones.
	class OSKAPI_CALL IBottomLevelCollider {

	public:

		virtual ~IBottomLevelCollider() = default;

		OSK_DEFINE_AS(IBottomLevelCollider);


		/// @brief Comprueba si este área de colisión está en contacto
		/// con otra, y devuelve la información detallada resultante.
		/// 
		/// @param other Otra área de colisión de nivel bajo.
		/// @param transformA Transform de este collider.
		/// @param transformB Transform del otro collider.
		/// 
		/// @return Información detallada de la colisión.
		virtual DetailedCollisionInfo GetCollisionInfo(const IBottomLevelCollider& other,
			const ECS::Transform3D& transformA, const ECS::Transform3D& transformB) const = 0;

		/// @brief Obtiene el punto más lejano del collider en la dirección dada.
		/// @param direction Dirección.
		/// @return Punto del collider más lejano respecto al centro en esa dirección.
		virtual Vector3f GetFurthestPoint(Vector3f direction) const = 0;

		virtual bool ContainsPoint(const Vector3f& point) const = 0;

	};

}
