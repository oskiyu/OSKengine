#pragma once

namespace OSK {

	enum class GpuImageLayout {
		UNDEFINED,
		PRESENT,
		COLOR_ATTACHMENT,
		DEPTH_STENCIL_TARGET,
		DEPTH_STENCIL_READ_ONLY,
		SHADER_READ_ONLY,
		TRANSFER_SOURCE,
		TRANSFER_DESTINATION
	};

}