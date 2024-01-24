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

#include "Serializer.h"


namespace OSK::ECS {
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

		template <typename T>
		friend nlohmann::json PERSISTENCE::SerializeJson<T>(const T& data);

		template <typename T>
		friend T PERSISTENCE::DeserializeJson<T>(const nlohmann::json& data);

	public:

		/// @brief Operador de copia.
		/// @param other Otro collider que será copiado.
		void CopyFrom(const Collider& other);

		/// @brief Establece el volumen de nivel alto.
		void SetTopLevelCollider(OwnedPtr<ITopLevelCollider> collider);
		/// @brief Añade un volumen de nivel bajo.
		void AddBottomLevelCollider(OwnedPtr<IBottomLevelCollider> collider);

		/// @brief Comprueba si hay colisión entre dos colliders.
		/// @param other Otro collider.
		/// @param thisTransform Transform de este collider en el instante actual.
		/// @param otherTransform Transform del otro collider en el instante actual.
		/// @return 
		CollisionInfo GetCollisionInfo(const Collider& other, 
			const ECS::Transform3D& thisTransform, const ECS::Transform3D& otherTransform) const;

		const ITopLevelCollider* GetTopLevelCollider() const { return m_topLevelCollider.GetPointer(); }
		ITopLevelCollider* GetTopLevelCollider() { return m_topLevelCollider.GetPointer(); }

		const IBottomLevelCollider* GetBottomLevelCollider(UIndex32 id) const { return m_bottomLevelColliders[id].GetPointer(); }
		IBottomLevelCollider* GetBottomLevelCollider(UIndex32 id) { return m_bottomLevelColliders[id].GetPointer(); }

		USize32 GetBottomLevelCollidersCount() const;

	private:

		UniquePtr<ITopLevelCollider> m_topLevelCollider;
		DynamicArray<UniquePtr<IBottomLevelCollider>> m_bottomLevelColliders;

	};

}

namespace OSK::PERSISTENCE {

	template <>
	nlohmann::json SerializeJson<OSK::COLLISION::Collider>(const OSK::COLLISION::Collider& data);

	template <>
	OSK::COLLISION::Collider DeserializeJson<OSK::COLLISION::Collider>(const nlohmann::json& json);

}
