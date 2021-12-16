#pragma once

#include <optional>
#include "OSKmacros.h"

namespace OSK {

	/// <summary>
	/// Contiene informaci�n sobre qu� familias de comandos soporta una GPU.
	/// </summary>
	struct OSKAPI_CALL QueueFamilyIndices {

		/// <summary>
		/// Cola gr�fica.
		/// </summary>
		std::optional<uint32_t> graphicsFamily;

		/// <summary>
		/// Cola de presentaci�n.
		/// (Pueden no ser la misma).
		/// </summary>
		std::optional<uint32_t> presentFamily;

		/// <summary>
		/// �Hay alguna cola?
		/// </summary>
		/// <returns>True si hay alguna cola disponible.</returns>
		bool IsComplete() const;

	};

}
