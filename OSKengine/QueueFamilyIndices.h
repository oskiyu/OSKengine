#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <optional>
#include <cstdint>

namespace OSK::VULKAN {

	struct OSKAPI_CALL QueueFamilyIndices {

		//Cola gr�fica.
		std::optional<uint32_t> GraphicsFamily;

		//Cola de presentaci�n.
		//(Pueden no ser la misma).
		std::optional<uint32_t> PresentFamily;

		//�Hay alguna cola?
		bool IsComplete() const;

	};

}