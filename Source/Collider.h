// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "ApiCall.h"
#include "UniquePtr.hpp"
#include "DynamicArray.hpp"

#include "BroadColliderHolder.h"
#include "NarrowColliderHolder.h"

#include "CollisionInfo.h"

#include "Component.h"

#include "Serializer.h"


namespace OSK {
	class Transform3D;
}

namespace OSK::COLLISION {

	/// @brief Representa un volumen que puede interseccionar con
	/// otros.
	/// 
	/// El volumen está representado en dos capas: una capa de alto
	/// nivel representada por un volumen que cubre toda la geometría,
	/// y una capa de bajo nivel con varios volúmenes más ajustados
	/// y detallados.
	class OSKAPI_CALL Collider {

	public:

		OSK_SERIALIZABLE();

	public:

		/// @brief Operador de copia.
		/// @param other Otro collider que será copiado.
		void CopyFrom(const Collider& other);


		/// @brief Establece el volumen de nivel alto.
		void SetTopLevelCollider(UniquePtr<BroadColliderHolder>&& collider);

		/// @brief Añade un volumen de nivel bajo.
		void AddBottomLevelCollider(UniquePtr<NarrowColliderHolder>&& collider);

		const BroadColliderHolder* GetTopLevelCollider() const { return m_topLevelCollider.GetPointer(); }
		BroadColliderHolder* GetTopLevelCollider() { return m_topLevelCollider.GetPointer(); }

		const NarrowColliderHolder* GetBottomLevelCollider(UIndex32 id) const { return m_bottomLevelColliders[id].GetPointer(); }
		NarrowColliderHolder* GetBottomLevelCollider(UIndex32 id) { return m_bottomLevelColliders[id].GetPointer(); }

		USize32 GetBottomLevelCollidersCount() const;

	private:

		UniquePtr<BroadColliderHolder> m_topLevelCollider;
		DynamicArray<UniquePtr<NarrowColliderHolder>> m_bottomLevelColliders;

	};

}

OSK_SERIALIZATION(OSK::COLLISION::Collider);
