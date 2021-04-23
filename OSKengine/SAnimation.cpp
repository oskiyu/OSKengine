#include "SAnimation.h"

using namespace OSK::Animation;

void SAnimation::Clear() {
	for (uint32_t i = 0; i < NumberOfChannels; i++)
		BoneChannels[i].Clear();

	if (BoneChannels)
		delete[] BoneChannels;
}