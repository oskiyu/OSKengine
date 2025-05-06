#pragma once

#include "IBroadCollisionDetector.h"

namespace OSK::COLLISION {

	class OSKAPI_CALL SphereSphereBroadCollisionDetector final : public IBroadCollisionDetector {

	public:

		std::optional<bool> GetCollision(
			const BroadColliderHolder& first,
			const BroadColliderHolder& second) const override;

	};

}
