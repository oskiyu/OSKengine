#pragma once

#include "ApiCall.h"
#include "IBroadCollisionDetector.h"

#include "DynamicArray.hpp"
#include "UniquePtr.hpp"

namespace OSK::COLLISION {

	/// @brief Contiene una lista de detectors
	/// y, cuando se quiere comprobar una colisi�n,
	/// ejecuta el detector conveniente.
	class OSKAPI_CALL BroadCollisionDetectorDispatcher final {

	public:

		/// @brief A�ade un nuevo detector, que tendr�
		/// prioridad sobre los detectores anteriores.
		/// @param detector Detector a a�adir.
		void AddDetector(UniquePtr<IBroadCollisionDetector>&& detector);

		/// @brief Comprueba si hay alguna colisi�n
		/// entre dos colliders.
		/// @param first Primer collider.
		/// @param second Segundo collider.
		/// @return Resultado de la comprobaci�n.
		bool GetCollision(
			const BroadColliderHolder& first,
			const BroadColliderHolder& second) const;

	private:

		DynamicArray<UniquePtr<IBroadCollisionDetector>> m_detectors;

	};

}
