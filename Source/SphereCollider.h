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

		OSK_SERIALIZABLE();

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

		float m_radius = 1.0f;

	};

}

OSK_SERIALIZATION(OSK::COLLISION::SphereCollider);
