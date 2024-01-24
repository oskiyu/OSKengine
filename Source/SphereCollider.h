// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "ITopLevelCollider.h"

#include "Serializer.h"


namespace OSK::COLLISION {

	/// @brief Un área de colisión de alto nivel representado
	/// por una esfera.
	/// 
	/// Por defecto, tiene radio 1.
	class OSKAPI_CALL SphereCollider : public ITopLevelCollider {

	public:

		template <typename T>
		friend nlohmann::json PERSISTENCE::SerializeJson<T>(const T& data);

		template <typename T>
		friend T PERSISTENCE::DeserializeJson<T>(const nlohmann::json& data);

	public:

		/// @brief Esfera con radio 1.
		SphereCollider() = default;
		SphereCollider(float radius);

		OwnedPtr<ITopLevelCollider> CreateCopy() const override;


		void SetRadius(float radius);
		float GetRadius() const;

		bool ContainsPoint(const Vector3f& point) const override;

		bool IsBehindPlane(Plane plane) const override;

		RayCastResult CastRay(const Ray& ray) const override;
		bool IsColliding(const ITopLevelCollider& other) const override;

	private:

		float radius = 1;

	};

}

namespace OSK::PERSISTENCE {

	template <>
	nlohmann::json SerializeJson<OSK::COLLISION::SphereCollider>(const OSK::COLLISION::SphereCollider& data);

	template <>
	OSK::COLLISION::SphereCollider DeserializeJson<OSK::COLLISION::SphereCollider>(const nlohmann::json& json);

}
