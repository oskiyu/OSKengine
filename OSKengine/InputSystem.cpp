#include "InputSystem.h"

using namespace OSK;

void InputSystem::SetWindow(WindowAPI* window) {
	this->window = window;
}

Signature InputSystem::GetSystemSignature() {
	Signature signature;

	signature.set(ECSsystem->GetComponentType<OSK::InputComponent>());

	return signature;
}

void InputSystem::OnTick(deltaTime_t deltaTime) {
	window->UpdateKeyboardState(NewKS);
	window->UpdateMouseState(NewMS);

	for (auto& i : events) {
		auto& desc = i.second;

		for (auto key : desc.LinkedKeys) {
			if (NewKS.IsKeyDown(key)) {
				executeFunction(desc.EventName, deltaTime);

				break;
			}
		}

		for (auto button : desc.LinkedButtons) {
			if (NewMS.IsButtonDown(button)) {
				executeFunction(desc.EventName, deltaTime);

				break;
			}
		}
	}

	for (auto& i : oneTimeEvents) {
		auto& desc = i.second;

		for (auto key : desc.LinkedKeys) {
			if (NewKS.IsKeyDown(key) && OldKS.IsKeyUp(key)) {
				executeOneTimeFunction(desc.EventName);

				break;
			}
		}

		for (auto button : desc.LinkedButtons) {
			if (NewMS.IsButtonDown(button) && OldMS.IsButtonUp(button)) {
				executeOneTimeFunction(desc.EventName);

				break;
			}
		}
	}

	window->UpdateKeyboardState(OldKS);
	window->UpdateMouseState(OldMS);
}

void InputSystem::executeFunction(std::string& name, deltaTime_t deltaTime) {
	for (auto& i : Objects) {
		auto& input = ECSsystem->GetComponent<InputComponent>(i);

		input.ExecuteInputFunction(name, deltaTime);
	}
}

void InputSystem::executeOneTimeFunction(std::string& name) {
	for (auto& i : Objects) {
		auto& input = ECSsystem->GetComponent<InputComponent>(i);

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