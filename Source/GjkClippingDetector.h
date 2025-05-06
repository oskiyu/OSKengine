#pragma once

#include "IDetailedCollisionDetector.h"

namespace OSK::COLLISION {

	/// @brief Usa el algoritmo GJK para detección
	/// de colisiones, y después hace clipping
	/// sobre las caras de los colliders para
	/// obtener una serie de puntos de contacto.
	/// 
	/// @pre @p first  es de tipo `ConvexCollider`.
	/// @pre @p second es de tipo `ConvexCollider`.
	class OSKAPI_CALL GjkClippingDetector final : public IDetailedCollisionDetector {

	public:

		std::optional<DetailedCollisionInfo> GetCollision(
			const NarrowColliderHolder& first,
			const NarrowColliderHolder& second) const override;

	};

}
