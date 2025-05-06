#pragma once

#include "IDetailedCollisionDetector.h"

namespace OSK::COLLISION {

	/// @brief Detector para el caso especial
	/// de dos esferas, debido a ofrecer una soluci�n
	/// muy r�pida.
	class OSKAPI_CALL DetailedSphereSphereDetector final : public IDetailedCollisionDetector {

	public:

		std::optional<DetailedCollisionInfo> GetCollision(
			const NarrowColliderHolder& first,
			const NarrowColliderHolder& second) const override;

	};

}
