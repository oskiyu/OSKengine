#pragma once

#include "ApiCall.h"
#include <optional>

namespace OSK::COLLISION {

	class BroadColliderHolder;


	/// @brief Interfaz para un objeto que permite
	/// detectar una colisi�n entre dos colliders.
	/// 
	/// Permite configurar distintos algoritmos 
	/// dependiendo del tipo de los colliders.
	class OSKAPI_CALL IBroadCollisionDetector {

	public:

		virtual ~IBroadCollisionDetector() = default;

		/// @param first Primer collider.
		/// @param second Segundo collider.
		/// @return Resultado de la detecci�n
		/// de colisi�n.
		/// 
		/// Si este detector no puede detectar
		/// la colisi�n para los tipos de los
		/// colliders, devuelve vac�o.
		virtual std::optional<bool> GetCollision(
			const BroadColliderHolder& first,
			const BroadColliderHolder& second) const = 0;

	};

}
