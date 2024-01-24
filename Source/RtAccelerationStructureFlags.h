#pragma once

#include "EnumFlags.hpp"

namespace OSK::ASSETS {
	enum class ModelMobilityType;
}

namespace OSK::GRAPHICS {

	enum class RtAccelerationStructureFlags {
		FAST_TRACE,
		FAST_BUILD,
		ALLOW_UPDATE
	};

	enum class RtAccelerationStructureUpdateType {
		UPDATE,
		REBUILD
	};

	RtAccelerationStructureFlags GetAsBuildFlags(ASSETS::ModelMobilityType);

}

OSK_FLAGS(OSK::GRAPHICS::RtAccelerationStructureFlags);
