#pragma once

#include "ApiCall.h"
#include "IDetailedCollisionDetector.h"
#include "DetailedCollisionInfo.h"

#include "DynamicArray.hpp"
#include "UniquePtr.hpp"

namespace OSK::COLLISION {

	class NarrowColliderHolder;

	/// @brief Contiene una lista de detectors
	/// y, cuando se quiere comprobar una colisión,
	/// ejecuta el detector conveniente.
	class OSKAPI_CALL DetailedCollisionDetectorDispatcher final {

	public:

		/// @brief Añade un nuevo detector, que tendrá
		/// prioridad sobre los detectores anteriores.
		/// @param detector Detector a añadir.
		void AddDetector(UniquePtr<IDetailedCollisionDetector>&& detector);

		/// @brief Comprueba si hay alguna colisión
		/// entre dos colliders.
		/// @param first Primer collider.
		/// @param second Segundo collider.
		/// @return Resultado de la comprobación.
		DetailedCollisionInfo GetCollision(
			const NarrowColliderHolder& first,
			const NarrowColliderHolder& second) const;

	private:

		DynamicArray<UniquePtr<IDetailedCollisionDetector>> m_detectors;

	};

}
