// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "IDisplay.h"

using namespace OSK;
using namespace OSK::IO;


bool IDisplay::IsOpen() const { 
	return isOpen; 
}

Vector2ui IDisplay::GetResolution() const { 
	return resolution; 
}

float IDisplay::GetScreenRatio() const {
	return resolution.y != 0
		? static_cast<float>(resolution.x) / static_cast<float>(resolution.y)
		: 1.0f;
}

USize32 IDisplay::GetRefreshRate() const {

}
