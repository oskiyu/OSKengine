#pragma once

#include "IDetailedCollisionDetector.h"

namespace OSK::COLLISION {

	/// @brief Usa el algoritmo GJK para detecci�n
	/// de colisiones, y el algoritmo EPA para obtener
	/// el MTV y un punto de contacto.
	/// 
	/// Todos los `IBottomLevelCollider` son tambi�n
	/// `IGjkCollider`, por lo que este detector es
	/// compatible con todos los colliders, 
	/// independientemente de su tipo.
	/// 
	/// Por ello, es el detector que se usa por defecto.
	class OSKAPI_CALL GjkEpaDetector final : public IDetailedCollisionDetector {

	public:

		std::optional<DetailedCollisionInfo> GetCollision(
			const NarrowColliderHolder& first,
			const NarrowColliderHolder& second) const override;

	};

}
