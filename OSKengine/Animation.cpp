#include "Animation.h"

namespace OSK {

	Animation::Animation() {
		Name = "unknown";
	}

	Animation::Animation(const std::string& name, const float_t& startTime, const float_t& endTime, const float_t& speed, const int32_t& priority, const bool& loop) {
		Name = name;
		StartTime = startTime;
		EndTime = endTime;
		Speed = speed;
		Priority = priority;
		Loop = loop;
	}

	Animation::~Animation() {

	}

}