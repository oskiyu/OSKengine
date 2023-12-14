#pragma once

#include <OSKengine/Component.h>

struct CarAiComponent {

	OSK_COMPONENT("CarAiComponent");
	UIndex64 curentPointId = 70;
	float diferencia = 1.0f;
	OSK::Vector3f nextPoint = OSK::Vector3f::Zero;

};
