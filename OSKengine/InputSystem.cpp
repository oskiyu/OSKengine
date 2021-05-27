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
				executeFunction(desc.eventName, deltaTime);

				break;
			}
		}

		for (auto button : desc.linkedButtons) {
			if (newMouseState.IsButtonDown(button)) {
				executeFunction(desc.eventName, deltaTime);

				break;
			}
		}

		for (gamepadId_t gamepadId = 0; gamepadId < 4; gamepadId++) {
			const auto gamepad = window->GetGamepadState(gamepadId);

			if (gamepadId != desc.gamepadCode)
				continue;

			for (auto button : desc.linkedGamepadButtons) {
				if (gamepad.IsButtonDown(button)) {
					executeFunction(desc.eventName, deltaTime);

					break;
				}
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

		for (gamepadId_t gamepadId = 0; gamepadId < 4; gamepadId++) {
			const auto gamepad = window->GetGamepadState(gamepadId);

			if (gamepadId != desc.gamepadCode)
				continue;

			for (auto button : desc.linkedGamepadButtons) {
				if (gamepad.IsButtonDown(button)) {
					executeFunction(desc.eventName, deltaTime);

					break;
				}
			}
		}
	}

	for (auto& i : axisEvents) {
		auto& desc = i.second;

		for (gamepadId_t gamepadId = 0; gamepadId < 4; gamepadId++) {
			const auto gamepad = window->GetGamepadState(gamepadId);

			if (gamepadId != desc.gamepadCode)
				continue;

			for (auto axis : desc.linkedAxes)
				executeAxisFunction(desc.eventName, deltaTime, gamepad.GetAxisState(axis));

			for (auto buttonAxis : desc.linkedGamepadAxes) {
				float value = 0.0f;

				if (newKeyboardState.IsKeyDown(buttonAxis.keyA))
					value += buttonAxis.valueA;
				
				if (newKeyboardState.IsKeyDown(buttonAxis.keyB))
					value += buttonAxis.valueB;

				executeAxisFunction(desc.eventName, deltaTime, value);
			}
		}
	}

	window->UpdateKeyboardState(oldKeyboardState);
	window->UpdateMouseState(oldMouseState);
}

void InputSystem::executeFunction(std::string& name, deltaTime_t deltaTime) {
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

void InputSystem::executeAxisFunction(std::string& name, deltaTime_t deltaTime, float axis) {
	for (auto& i : objects) {
		auto& input = entityComponentSystem->GetComponent<InputComponent>(i);

		input.ExecuteAxisInputFunction(name, deltaTime, axis);
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

AxisInputEvent& InputSystem::GetAxisInputEvent(const std::string& e) {
	return axisEvents[e];
}

void InputSystem::RegisterAxisInputEvent(const std::string& name) {
	axisEvents.insert({ name, AxisInputEvent{ name } });
}