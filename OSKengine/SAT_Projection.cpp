#include "SAT_Projection.hpp"

using namespace OSK::Collision;

SAT_Projection::SAT_Projection(float min, float max){
	this->min = min;
	this->max = max;
}

bool SAT_Projection::Overlaps(const SAT_Projection& proj) const {
	return max > proj.min && min < proj.max;
}

float SAT_Projection::GetOverlap(const SAT_Projection& proj) const {
	if (!Overlaps(proj))
		return 0.0f;

	float output = 0.0f;
	output = max - proj.min;

	const float out = min - proj.max;
	if (std::abs(out) < std::abs(output))
		output = out;

	return output;
}
