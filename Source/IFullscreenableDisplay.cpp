// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#include "IFullscreenableDisplay.h"

using namespace OSK;
using namespace OSK::IO;

bool IFullscreenableDisplay::IsFullscreen() const {
	return isFullscreen;
}

void IFullscreenableDisplay::ToggleFullscreen() {
	SetFullscreen(!isFullscreen);
}
