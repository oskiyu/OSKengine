// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "IBroadCollider.h"
#include "INarrowCollider.h"

#include "Serializer.h"


namespace OSK::COLLISION {

	/// @brief Un área de colisión de alto nivel representado
	/// por una esfera.
	/// 
	/// Por defecto, tiene radio 1.
	class OSKAPI_CALL SphereCollider : public IBroadCollider, public INarrowCollider {

	public:

		OSK_SERIALIZABLE();

	public:

		/// @brief Esfera con radio 1.
		explicit SphereCollider() = default;
		explicit SphereCollider(float radius);

		UniquePtr<IBroadCollider>  CreateBroadCopy() const override;
		UniquePtr<INarrowCollider> CreateNarrowCopy() const override;

		void SetRadius(float radius);
		float GetRadius() const;

		RayCastResult CastRay(const Ray& ray) const override;

		GjkSupport GetSupport(const Vector3f& direction) const override;

	private:

		float m_radius = 1.0f;

	};

}

OSK_SERIALIZATION(OSK::COLLISION::SphereCollider);
