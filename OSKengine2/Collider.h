// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "DynamicArray.hpp"

#include "ITopLevelCollider.h"
#include "IBottomLevelCollider.h"
#include "CollisionInfo.h"
#include "OwnedPtr.h"

#include "Component.h"

namespace OSK::ECS {
	class Transform3D;
}

namespace OSK::COLLISION {

	/// @brief Representa un volumen que puede interseccionar con
	/// otros.
	/// 
	/// El volumen est� representado en dos capas: una capa de alto
	/// nivel representada por un volumen que cubre toda la geometr�a,
	/// y una capa de bajo nivel con varios vol�menes m�s ajustados
	/// y detallados.
	class OSKAPI_CALL Collider {

	public:

		OSK_COMPONENT("OSK::Collider");

		/// @brief Establece el volumen de nivel alto.
		void SetTopLevelCollider(OwnedPtr<ITopLevelCollider> collider);
		/// @brief A�ade un volumen de nivel bajo.
		void AddBottomLevelCollider(OwnedPtr<IBottomLevelCollider> collider);

		/// @brief Comprueba si hay colisi�n entre dos colliders.
		/// @param other Otro collider.
		/// @param thisTransform Transform de este collider en el instante actual.
		/// @param otherTransform Transform del otro collider en el instante actual.
		/// @return 
		CollisionInfo GetCollisionInfo(const Collider& other, 
			const ECS::Transform3D& thisTransform, const ECS::Transform3D& otherTransform) const;

		ITopLevelCollider* GetTopLevelCollider() const;
		IBottomLevelCollider* GetBottomLevelCollider(TIndex id) const;

		TSize GetBottomLevelCollidersCount() const;

	private:

		UniquePtr<ITopLevelCollider> topLevelCollider;
		DynamicArray<UniquePtr<IBottomLevelCollider>> bottomLevelColliders;

	};

}