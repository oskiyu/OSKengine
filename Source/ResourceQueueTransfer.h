#pragma once

#include "ApiCall.h"
#include "QueueFamilyIndices.h"

namespace OSK::GRAPHICS {

	class ICommandQueue;

	/// @brief Estructura que permite expresar una transferencia
	/// de un recurso de una cola a otra.
	/// 
	/// De esta manera, el recurso usado en una cola
	/// podr� ser usado en otra.
	/// 
	/// Una vez transferido, el recurso no podr�
	/// usarse en la cola inicial al no ser que se
	/// vuelva a transferir a esta.
	struct OSKAPI_CALL ResourceQueueTransferInfo {

		/// @return Estructura indicando que no hay ninguna transferencia.
		static ResourceQueueTransferInfo Empty();

		/// @param source Cola de origen, en la que el recurso se encuentra actualmente.
		/// @param destination Cola en la que se encontrar� el recurso despu�s de la operaci�n.
		/// @return Estructura completa.
		/// 
		/// @post @p transfer ser� `true`.
		static ResourceQueueTransferInfo FromTo(const ICommandQueue* source, const ICommandQueue* destination);

		/// @brief Fammilia de origen.
		/// @pre @p transfer debe ser `true`.
		const ICommandQueue* sourceQueue = nullptr;

		/// @brief Fammilia de destino.
		/// @pre @p transfer debe ser `true`.
		const ICommandQueue* destinationQueue = nullptr;

		/// @brief Indica si hay una transferencia o no.
		bool transfer = false;

	private:

		explicit ResourceQueueTransferInfo() = default;

	};

}
