#pragma once

#include "OSKmacros.h"
#include "DynamicArray.hpp"
#include "CommandQueueSupport.h"

namespace OSK::GRAPHICS {

	/// @brief Informaci�n sobre una familia de colas de comandos.
	struct QueueFamily {
		/// @brief �ndice de la familia.
		TIndex familyIndex;
		/// @brief N�mero de colas en la familia.
		TSize numQueues;
		/// @brief Operaciones soportadas.
		CommandQueueSupport support;
	};
		
	/// @brief Informaci�n sobre las colas de comandos soportadas
	/// por una GPU.
	struct OSKAPI_CALL QueueFamiles {

		/// @brief Familias soportadas.
		DynamicArray<QueueFamily> families;
	
		/// @brief Devuelve una lista con todas las familias que tienen el soporte dado.
		/// @param support Soporte requerido.
		/// @return Lista (puede estar vac�a si no hay familias que lo soporten).
		DynamicArray<QueueFamily> GetFamilies(CommandQueueSupport support) const;


		/// @brief Comprueba si una GPU tiene soporta para todas las colas
		/// que necesitamos.
		/// @return True si es compatible.
		bool IsComplete() const;

	};

}
