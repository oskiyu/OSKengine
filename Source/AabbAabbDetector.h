#pragma once

#include "IBroadCollisionDetector.h"

namespace OSK::COLLISION {

	/// @brief Detector para el caso especial
	/// de dos esferas, debido a ofrecer una soluci�n
	/// muy r�pida.
	class OSKAPI_CALL AabbAabbCollisionDetector final : public IBroadCollisionDetector {

	public:

		std::optional<bool> GetCollision(
			const BroadColliderHolder& first,
			const BroadColliderHolder& second) const override;

	};

}
