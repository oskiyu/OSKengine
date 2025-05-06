#pragma once

#include "ApiCall.h"

#include "DetailedCollisionInfo.h"
#include <optional>

namespace OSK::COLLISION {

	class NarrowColliderHolder;


	/// @brief Interfaz para un objeto que permite
	/// detectar una colisión entre dos colliders.
	/// 
	/// Permite configurar distintos algoritmos 
	/// dependiendo del tipo de los colliders.
	class OSKAPI_CALL IDetailedCollisionDetector {

	public:

		virtual ~IDetailedCollisionDetector() = default;

		/// @param first Primer collider.
		/// @param second Segundo collider.
		/// @return Resultado de la detección
		/// de colisión.
		/// 
		/// Si este detector no puede detectar
		/// la colisión para los tipos de los
		/// colliders, devuelve vacío.
		virtual std::optional<DetailedCollisionInfo> GetCollision(
			const NarrowColliderHolder& first,
			const NarrowColliderHolder& second) const = 0;

	};

}
