#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>

namespace OSK::VULKAN {

	/// <summary>
	/// Dependencias de los subpasses: cosas que tienen que pasar antes.
	/// </summary>
	class OSKAPI_CALL SubpassDependency {

	public:

		/// <summary>
		/// Dependencia nativa.
		/// </summary>
		VkSubpassDependency VulkanDependency;

	};

}