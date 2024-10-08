#pragma once

#include "Component.h"

#include "Collider.h"

#include "AssetRef.h"
#include "PreBuiltCollider.h"

#include "Serializer.h"


namespace OSK::ECS {

	class OSKAPI_CALL CollisionComponent {

	public:

		OSK_SERIALIZABLE_COMPONENT();

	public:

		OSK_COMPONENT("OSK::CollisionComponent");

		void SetCollider(ASSETS::AssetRef<ASSETS::PreBuiltCollider> preBuiltCollider);
		void SetCollider(const COLLISION::Collider& collider);

		COLLISION::Collider* GetCollider();
		const COLLISION::Collider* GetCollider() const;

	private:

		COLLISION::Collider m_customCollider;
		ASSETS::AssetRef<ASSETS::PreBuiltCollider> m_preBuiltCollider;

	};

}

OSK_COMPONENT_SERIALIZATION(OSK::ECS::CollisionComponent);
