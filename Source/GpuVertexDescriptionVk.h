#pragma once

#include "Platforms.h"
#ifdef OSK_USE_VULKAN_BACKEND

#include "IGpuVertexDescription.h"

#include <vulkan/vulkan.h>
#include "DynamicArray.hpp"

namespace OSK::GRAPHICS {

	class VertexInfo;

	class OSKAPI_CALL GpuVertexDescriptionVk final : public IGpuVertexDescription {

	public:

	};

}

#endif
