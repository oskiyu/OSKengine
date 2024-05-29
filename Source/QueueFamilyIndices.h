#pragma once

#include "ApiCall.h"
#include "DynamicArray.hpp"
#include "CommandsSupport.h"

namespace OSK::GRAPHICS {

	/// @brief Informaci�n sobre una familia de colas de comandos.
	struct QueueFamily {
		/// @brief �ndice de la familia.
		UIndex32 familyIndex;
		/// @brief N�mero de colas en la familia.
		USize32 numQueues;
		/// @brief Operaciones soportadas.
		CommandsSupport support;
	};
		
	/// @brief Informaci�n sobre las colas de comandos soportadas
	/// por una GPU.
	struct OSKAPI_CALL QueueFamiles {

		/// @brief Familias soportadas.
		DynamicArray<QueueFamily> families;
	
		/// @brief Devuelve una lista con todas las familias que tienen el soporte dado.
		/// @param support Soporte requerido.
		/// @return Lista (puede estar vac�a si no hay familias que lo soporten).
		DynamicArray<QueueFamily> GetFamilies(CommandsSupport support) const;


		/// @brief Comprueba si una GPU tiene soporta para todas las colas
		/// que necesitamos.
		/// @return True si es compatible.
		bool IsComplete() const;

	};

}
