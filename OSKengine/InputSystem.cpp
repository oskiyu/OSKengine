#include "InputSystem.h"

using namespace OSK;

void InputSystem::SetWindow(WindowAPI* window) {
	this->window = window;
}

Signature InputSystem::GetSystemSignature() {
	Signature signature;

	signature.set(entityComponentSystem->GetComponentType<OSK::InputComponent>());

	return signature;
}

void InputSystem::OnTick(deltaTime_t deltaTime) {
	window->UpdateKeyboardState(newKeyboardState);
	window->UpdateMouseState(newMouseState);

	for (auto& i : events) {
		auto& desc = i.second;

		for (auto key : desc.linkedKeys) {
			if (newKeyboardState.IsKeyDown(key)) {
				ExecuteFunction(desc.eventName, deltaTime);

				break;
			}
		}

		for (auto button : desc.linkedButtons) {
			if (newMouseState.IsButtonDown(button)) {
				ExecuteFunction(desc.eventName, deltaTime);

				break;
			}
		}
	}

	for (auto& i : oneTimeEvents) {
		auto& desc = i.second;

		for (auto key : desc.linkedKeys) {
			if (newKeyboardState.IsKeyDown(key) && oldKeyboardState.IsKeyUp(key)) {
				executeOneTimeFunction(desc.eventName);

				break;
			}
		}

		for (auto button : desc.linkedButtons) {
			if (newMouseState.IsButtonDown(button) && oldMouseState.IsButtonUp(button)) {
				executeOneTimeFunction(desc.eventName);

				break;
			}
		}
	}

	window->UpdateKeyboardState(oldKeyboardState);
	window->UpdateMouseState(oldMouseState);
}

void InputSystem::ExecuteFunction(std::string& name, deltaTime_t deltaTime) {
	for (auto& i : objects) {
		auto& input = entityComponentSystem->GetComponent<InputComponent>(i);

		input.ExecuteInputFunction(name, deltaTime);
	}
}

void InputSystem::executeOneTimeFunction(std::string& name) {
	for (auto& i : objects) {
		auto& input = entityComponentSystem->GetComponent<InputComponent>(i);

		input.ExecuteOneTimeInputFunction(name);
	}
}

InputEvent& InputSystem::GetInputEvent(const std::string& e) {
	return events[e];
}

void InputSystem::RegisterInputEvent(const std::string& name) {
	events.insert({ name, InputEvent{ name } });
}

OneTimeInputEvent& InputSystem::GetOneTimeInputEvent(const std::string& e) {
	return oneTimeEvents[e];
}

void InputSystem::RegisterOneTimeInputEvent(const std::string& name) {
	oneTimeEvents.insert({ name, OneTimeInputEvent{ name } });
}