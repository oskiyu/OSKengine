#include "ProfilingUnit.h"

#include <glfw3.h>

namespace OSK {

	ProfilingUnit::ProfilingUnit() {

	}

	ProfilingUnit::ProfilingUnit(const std::string& name) {
		Name = name;
	}

	void ProfilingUnit::Start() {
		startTime = glfwGetTime();
	}

	void ProfilingUnit::End() {
		endTime = glfwGetTime();
		lastDuration = endTime - startTime;
	}

	deltaTime_t ProfilingUnit::GetValue() const {
		return lastDuration;
	}

}