#include "SAnimation.h"

using namespace OSK::Animation;

void SAnimation::Clear() {
	for (uint32_t i = 0; i < boneChannels.size(); i++)
		boneChannels[i].Clear();
}