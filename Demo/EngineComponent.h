#pragma once

#include <OSKengine/Component.h>
#include <OSKengine/IAudioSource.h>

#include <array>

struct EngineComponent {

	OSK_COMPONENT("EngineComponent");

	struct Gear {
		float ratio = 0.0f;
	};

	std::array<Gear, 7> gears = { {
		/*{2.66f},
		{ 1.78f },
		{ 1.30f },
		{ 1.0f },
		{ 0.74f },
		{ 0.50f }*/ 
		{ 2.10f},
		{ 1.3f },
		{ 0.9f },
		{ 0.7f },
		{ 0.55f },
		{ 0.45f },
		{ 0.35f },
	} };
	UIndex64 currentGear = 0;

	float GetCurrentTorqueMultiplier() const {
		return gears[currentGear].ratio * differentialRatio;
	}

	float differentialRatio = 3.66f;

	float currentRpm = 1000.0f;
	float minRpm = 800.0f;
	float maxRpm = 4200.0f;
	float changePoint = 2000.0f;

	OSK::UniquePtr<OSK::AUDIO::IAudioSource> audioSource;

};