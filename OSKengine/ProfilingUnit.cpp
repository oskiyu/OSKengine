#include "ProfilingUnit.h"

#include <GLFW/glfw3.h>

namespace OSK {

	ProfilingUnit::ProfilingUnit() {

	}

	ProfilingUnit::ProfilingUnit(const std::string& name) {
		this->name = name;
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