#pragma once

#include "IBroadCollisionDetector.h"

namespace OSK::COLLISION {

	/// @brief Detector de colisiones entre
	/// SphereCollider y AxisAlignedBoundingBox.
	/// 
	/// Para colisionadores broad.
	class OSKAPI_CALL SphereAabbCollisionDetector final : public IBroadCollisionDetector {

	public:

		std::optional<bool> GetCollision(
			const BroadColliderHolder& first,
			const BroadColliderHolder& second) const override;

	};

}
