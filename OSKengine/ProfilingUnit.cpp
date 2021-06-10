#include "ProfilingUnit.h"

#include <GLFW/glfw3.h>

namespace OSK {

	ProfilingUnit::ProfilingUnit() {

	}

	ProfilingUnit::ProfilingUnit(const std::string& name) {
		this->name = name;
	}

	void ProfilingUnit::Start() {
		startTime = (deltaTime_t)glfwGetTime();
	}

	void ProfilingUnit::End() {
		endTime = (deltaTime_t)glfwGetTime();
		lastDuration = endTime - startTime;
	}

	deltaTime_t ProfilingUnit::GetValue() const {
		return lastDuration;
	}

}