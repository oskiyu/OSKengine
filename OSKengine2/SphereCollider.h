// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "ITopLevelCollider.h"

namespace OSK::COLLISION {

	/// @brief Un área de colisión de alto nivel representado
	/// por una esfera.
	/// 
	/// POr defecto, tiene radio 1.
	class OSKAPI_CALL SphereCollider : public ITopLevelCollider {

	public:

		/// @brief Esfera con radio 1.
		SphereCollider() = default;
		SphereCollider(float radius);

		void SetRadius(float radius);
		float GetRadius() const;

		bool ContainsPoint(const Vector3f& thisOffset, const Vector3f& point) const override;

		RayCastResult CastRay(const Ray& ray, const Vector3f& position) const override;
		bool IsColliding(const ITopLevelCollider& other,
			const Vector3f& thisOffset, const Vector3f& otherOffset) const override;

	private:

		float radius = 1;

	};

}
