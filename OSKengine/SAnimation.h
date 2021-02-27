#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "SNodeAnim.h"

namespace OSK::Animation {

	struct OSKAPI_CALL SAnimation {
		void Clear() {
			for (uint32_t i = 0; i < NumberOfChannels; i++)
				BoneChannels[i].Clear();

			if (BoneChannels)
				delete[] BoneChannels;
		}

		std::string Name;

		deltaTime_t Duration;
		deltaTime_t TicksPerSecond;
		uint32_t NumberOfChannels;

		SNodeAnim* BoneChannels = nullptr;

	};

}
