#pragma once

#include "ApiCall.h"
#include "IDetailedCollisionDetector.h"
#include "DetailedCollisionInfo.h"

#include "DynamicArray.hpp"
#include "UniquePtr.hpp"

namespace OSK::COLLISION {

	class NarrowColliderHolder;

	/// @brief Contiene una lista de detectors
	/// y, cuando se quiere comprobar una colisi�n,
	/// ejecuta el detector conveniente.
	class OSKAPI_CALL DetailedCollisionDetectorDispatcher final {

	public:

		/// @brief A�ade un nuevo detector, que tendr�
		/// prioridad sobre los detectores anteriores.
		/// @param detector Detector a a�adir.
		void AddDetector(UniquePtr<IDetailedCollisionDetector>&& detector);

		/// @brief Comprueba si hay alguna colisi�n
		/// entre dos colliders.
		/// @param first Primer collider.
		/// @param second Segundo collider.
		/// @return Resultado de la comprobaci�n.
		DetailedCollisionInfo GetCollision(
			const NarrowColliderHolder& first,
			const NarrowColliderHolder& second) const;

	private:

		DynamicArray<UniquePtr<IDetailedCollisionDetector>> m_detectors;

	};

}
