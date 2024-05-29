#pragma once

#include "ApiCall.h"
#include "QueueFamilyIndices.h"

namespace OSK::GRAPHICS {

	/// @brief Estructura que permite expresar una transferencia
	/// de un recurso de una cola a otra.
	/// 
	/// De esta manera, el recurso usado en una cola
	/// podrá ser usado en otra.
	/// 
	/// Una vez transferido, el recurso no podrá
	/// usarse en la cola inicial al no ser que se
	/// vuelva a transferir a esta.
	struct OSKAPI_CALL ResourceQueueTransferInfo {

		/// @return Estructura indicando que no hay ninguna transferencia.
		static ResourceQueueTransferInfo Empty();

		/// @param source Cola de origen, en la que el recurso se encuentra actualmente.
		/// @param destination Cola en la que se encontrará el recurso después de la operación.
		/// @return Estructura completa.
		/// 
		/// @post @p transfer será `true`.
		static ResourceQueueTransferInfo FromTo(const QueueFamily& source, const QueueFamily& destination);

		/// @brief Fammilia de origen.
		/// @pre @p transfer debe ser `true`.
		QueueFamily sourceFamily;

		/// @brief Fammilia de destino.
		/// @pre @p transfer debe ser `true`.
		QueueFamily destinationFamily;

		/// @brief Indica si hay una transferencia o no.
		bool transfer = false;

	private:

		explicit ResourceQueueTransferInfo() = default;

	};

}
