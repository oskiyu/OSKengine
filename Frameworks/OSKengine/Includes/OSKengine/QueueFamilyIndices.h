#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <optional>
#include <cstdint>

namespace OSK::VULKAN {

	/// <summary>
	/// Contiene información sobre qué familias de comandos soporta una GPU.
	/// </summary>
	struct OSKAPI_CALL QueueFamilyIndices {

		/// <summary>
		/// Cola gráfica.
		/// </summary>
		std::optional<uint32_t> GraphicsFamily;

		/// <summary>
		/// Cola de presentación.
		/// (Pueden no ser la misma).
		/// </summary>
		std::optional<uint32_t> PresentFamily;

		/// <summary>
		/// ¿Hay alguna cola?
		/// </summary>
		/// <returns>True si hay alguna cola disponible.</returns>
		bool IsComplete() const;

	};

}