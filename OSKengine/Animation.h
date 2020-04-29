#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	class Animation {

	public:

		Animation();


		Animation(const std::string& name, const float_t& startTime, const float_t& endTime, const float_t& speed = 0.25f, const int32_t& priority = 10, const bool& loop = false);


		~Animation();


		std::string Name;


		float_t StartTime = 0.0f;


		float_t EndTime = 0.0f;


		float_t Speed = 0.25f;


		int32_t Priority = 10;


		bool Loop = false;


	private:

	};

}