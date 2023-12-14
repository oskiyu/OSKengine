#pragma once

#include <string>
#include <OSKengine/Event.h>

struct ExitEvent {
	OSK_EVENT("ExitEvent");
};

struct LoadLevelEvent {
	OSK_EVENT("LoadLevelEvent");

	std::string assetsPath;
};

struct PlayEvent {
	OSK_EVENT("PlayEvent");
};

struct PauseEvent {
	OSK_EVENT("PauseEvent");
};

struct SelectCarEvent {
	OSK_EVENT("SelectCar");

	std::string assetsPath;
};
