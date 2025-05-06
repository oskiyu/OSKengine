#pragma once

#include "ApiCall.h"
#include <optional>

namespace OSK::COLLISION {

	class BroadColliderHolder;


	/// @brief Interfaz para un objeto que permite
	/// detectar una colisión entre dos colliders.
	/// 
	/// Permite configurar distintos algoritmos 
	/// dependiendo del tipo de los colliders.
	class OSKAPI_CALL IBroadCollisionDetector {

	public:

		virtual ~IBroadCollisionDetector() = default;

		/// @param first Primer collider.
		/// @param second Segundo collider.
		/// @return Resultado de la detección
		/// de colisión.
		/// 
		/// Si este detector no puede detectar
		/// la colisión para los tipos de los
		/// colliders, devuelve vacío.
		virtual std::optional<bool> GetCollision(
			const BroadColliderHolder& first,
			const BroadColliderHolder& second) const = 0;

	};

}
