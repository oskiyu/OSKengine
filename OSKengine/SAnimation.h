#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "SNodeAnim.h"

namespace OSK::Animation {

	struct OSKAPI_CALL SAnimation {

		std::string Name;

		deltaTime_t Duration;
		deltaTime_t TicksPerSecond;
		uint32_t NumberOfChannels;

		std::vector<SNodeAnim> BoneChannels;

	};

	struct OSKAPI_CALL SAnimationHolder {
		std::vector<SAnimation> Animations;
	};

}
