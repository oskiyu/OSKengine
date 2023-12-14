#pragma once

#include <OSKengine/Component.h>

struct CarComponent {

	OSK_COMPONENT("CarComponent");
	
	float maxTurning = 30.0f;

	float GetMaxSpeedForCorner(float degrees) const {
		if (degrees < 10.0f) {
			return 110.0f / 3.6f;
		}
		else if (degrees < 40.0f) {
			return 50.0f / 3.6f;
		}
		else if (degrees < 60.0f) {
			return 10.0f / 3.6f;
		}
		else if (degrees < 90.0f) {
			return 5.0f / 3.6f;
		}
		else {
			return 1.0f / 3.6f;
		}
	}

};
