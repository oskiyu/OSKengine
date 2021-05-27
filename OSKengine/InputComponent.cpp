#include "InputComponent.h"

using namespace OSK;

std::function<void(deltaTime_t)>& InputComponent::GetInputFunction(const std::string& eventName) {
	if (inputEvents.find(eventName) == inputEvents.end())
		inputEvents.insert({ eventName, [](deltaTime_t deltaTime) {} });

	return inputEvents[eventName];
}

std::function<void()>& InputComponent::GetOneTimeInputFunction(const std::string& eventName) {
	if (oneTimeInputEvents.find(eventName) == oneTimeInputEvents.end())
		oneTimeInputEvents.insert({ eventName, []() {} });

	return oneTimeInputEvents[eventName];
}

std::function<void(deltaTime_t, float)>& InputComponent::GetAxisInputFunction(const std::string& eventName) {
	if (axisInputEvents.find(eventName) == axisInputEvents.end())
		axisInputEvents.insert({ eventName, [](deltaTime_t, float) {} });

	return axisInputEvents[eventName];
}

void InputComponent::ExecuteInputFunction(const std::string& eventName, deltaTime_t deltaTime) {
	if (inputEvents.find(eventName) != inputEvents.end())
		GetInputFunction(eventName)(deltaTime);
}

void InputComponent::ExecuteOneTimeInputFunction(const std::string& eventName) {
	if (oneTimeInputEvents.find(eventName) != oneTimeInputEvents.end())
		GetOneTimeInputFunction(eventName)();
}

void InputComponent::ExecuteAxisInputFunction(const std::string& eventName, deltaTime_t deltaTime, float axis) {
	if (axisInputEvents.find(eventName) != axisInputEvents.end())
		GetAxisInputFunction(eventName)(deltaTime, axis);
}
