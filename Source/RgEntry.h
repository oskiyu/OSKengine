#pragma once

#include "UniquePtr.hpp"
#include "RenderPass.h"
#include "RenderPassDependencies.h"

namespace OSK::GRAPHICS {

	/// @brief Conjunto de renderpass
	/// y sus dependencias con otros renderpasses.
	struct RenderPassEntry {
		UniquePtr<IRenderPass> renderPass;
		RenderPassDependencies dependencies;
	};

}
