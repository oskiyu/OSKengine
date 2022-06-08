module;

#include <OSKengine/OSKengine.h>

export module StopWatch;

export class StopWatch {

public:

	void Start() {
		startTime = OSK::Engine::GetCurrentTime();
	}

	void End(){
		endTime = OSK::Engine::GetCurrentTime();
	}

	float GetValue() const {
		return endTime - startTime;
	}

private:

	float startTime = 0;
	float endTime = 0;

};