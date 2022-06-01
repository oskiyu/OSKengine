#include "IInputListener.h"

#include "Math.h"

using namespace OSK::IO;
using namespace OSK::MATH;

void IInputListener::SetMinAxisValue(float value) {
	axisMin = Clamp(value, 0.0f, 1.0f);
}

float IInputListener::GetMinAxisValue() const {
	return axisMin;
}
