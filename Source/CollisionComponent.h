#pragma once

#include "Component.h"

#include "Collider.h"

#include "AssetRef.h"
#include "PreBuiltCollider.h"

#include "Serializer.h"


namespace OSK::ECS {

	class OSKAPI_CALL CollisionComponent {

	public:

		template <typename T>
		friend nlohmann::json PERSISTENCE::SerializeJson<T>(const T& data);

		template <typename T>
		friend T PERSISTENCE::DeserializeJson<T>(const nlohmann::json& data);

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

namespace OSK::PERSISTENCE {

	template <>
	nlohmann::json SerializeJson<OSK::ECS::CollisionComponent>(const OSK::ECS::CollisionComponent& data);

	template <>
	OSK::ECS::CollisionComponent DeserializeJson<OSK::ECS::CollisionComponent>(const nlohmann::json& json);

}
