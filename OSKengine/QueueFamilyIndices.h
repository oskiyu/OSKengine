#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <optional>
#include <cstdint>

namespace OSK::VULKAN {

	/// <summary>
	/// Contiene informaci�n sobre qu� familias de comandos soporta una GPU.
	/// </summary>
	struct OSKAPI_CALL QueueFamilyIndices {

		/// <summary>
		/// Cola gr�fica.
		/// </summary>
		std::optional<uint32_t> GraphicsFamily;

		/// <summary>
		/// Cola de presentaci�n.
		/// (Pueden no ser la misma).
		/// </summary>
		std::optional<uint32_t> PresentFamily;

		/// <summary>
		/// �Hay alguna cola?
		/// </summary>
		/// <returns>True si hay alguna cola disponible.</returns>
		bool IsComplete() const;

	};

}