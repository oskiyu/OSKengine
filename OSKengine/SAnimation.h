#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "SNodeAnim.h"

namespace OSK::Animation {

	struct OSKAPI_CALL SAnimation {
		~SAnimation() {
			//if (BoneChannels)
				//delete[] BoneChannels;
		}

		std::string Name;

		deltaTime_t Duration;
		deltaTime_t TicksPerSecond;
		uint32_t NumberOfChannels;

		SNodeAnim* BoneChannels = nullptr;

	};

	struct OSKAPI_CALL SAnimationHolder {
		std::vector<SAnimation> Animations;
	};

}
