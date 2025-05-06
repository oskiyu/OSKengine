#pragma once

#include "IBroadCollisionDetector.h"

namespace OSK::COLLISION {

	/// @brief Detector genérico, compatible para
	/// cualquier pareja de colisionadores de
	/// alto nivel.
	class OSKAPI_CALL BroadGjkCollisionDetector final : public IBroadCollisionDetector {

	public:

		std::optional<bool> GetCollision(
			const BroadColliderHolder& first,
			const BroadColliderHolder& second) const override;

	};

}
