// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include <limits>
#include "Vector3.hpp"

namespace OSK::COLLISION {

	class RayCastResult {

	public:

		static RayCastResult False() {
			return RayCastResult(false, 0.0f);
		}

		static RayCastResult True(const Vector3f& intersection) {
			return RayCastResult(true, intersection);
		}

		bool Result() const;
		const Vector3f& GetIntersectionPoint() const;

	private:

		RayCastResult(bool intersection, const Vector3f& distance) { }

		bool intersection = false;
		Vector3f intersectionPoint;

	};

}
