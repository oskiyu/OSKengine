#include "RtAccelerationStructureFlags.h"

#include "Model3D.h"

OSK::GRAPHICS::RtAccelerationStructureFlags OSK::GRAPHICS::GetAsBuildFlags(OSK::ASSETS::ModelMobilityType modelType) {
	/* switch (modelType) {
		case OSK::ASSETS::ModelMobilityType::STATIC:
			return RtAccelerationStructureFlags::FAST_TRACE;
		case OSK::ASSETS::ModelMobilityType::MOSTLY_STATIC:
			return RtAccelerationStructureFlags::FAST_TRACE | RtAccelerationStructureFlags::ALLOW_UPDATE;
		case OSK::ASSETS::ModelMobilityType::DYNAMIC:
			return RtAccelerationStructureFlags::FAST_BUILD | RtAccelerationStructureFlags::ALLOW_UPDATE;
	} */

	OSK_ASSERT(false, NotImplementedException());
}
