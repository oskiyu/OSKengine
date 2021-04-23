#include "SNodeAnim.h"

using namespace OSK::Animation;

void SNodeAnim::Clear() {
	if (PositionKeys)
		delete[] PositionKeys;
	if (PositionKeys)
		delete[] RotationKeys;
	if (PositionKeys)
		delete[] ScalingKeys;
}