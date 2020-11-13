#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <optional>
#include <cstdint>

namespace OSK::VULKAN {

	struct OSKAPI_CALL QueueFamilyIndices {

		//Cola gráfica.
		std::optional<uint32_t> GraphicsFamily;

		//Cola de presentación.
		//(Pueden no ser la misma).
		std::optional<uint32_t> PresentFamily;

		//¿Hay alguna cola?
		bool IsComplete() const;

	};

}