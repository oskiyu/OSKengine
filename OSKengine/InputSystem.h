#pragma once

#include "ECS.h"
#include "InputComponent.h"

#include <string>
#include "WindowAPI.h"

namespace OSK {

	class InputSystem : public ECS::System {

	public:

		void SetWindow(WindowAPI* window) {
			this->window = window;
		}

		void OnTick(deltaTime_t deltaTime) override {
			window->UpdateKeyboardState(NewKS);
			window->UpdateMouseState(NewMS);

			for (auto& i : events) {
				auto& desc = i.second;

				for (auto key : desc.LinkedKeys) {
					if (NewKS.IsKeyDown(key)) {
						executeFunction(desc.EventName);

						break;
					}
				}

				for (auto button : desc.LinkedButtons) {
					if (NewMS.IsButtonDown(button)) {
						executeFunction(desc.EventName);

						break;
					}
				}
			}

			window->UpdateKeyboardState(OldKS);
			window->UpdateMouseState(OldMS);
		}

		InputEvent& GetInputEvent(const std::string& e) {
			return events[e];
		}

		void RegisterInputEvent(const std::string& name) {
			events.insert({ name, InputEvent{ name } });
		}

	private:

		void executeFunction(std::string& name) {
			for (auto& i : Objects) {
				auto& input = ECSsystem->GetComponent<InputComponent>(i);

				input.ExecuteFunction(name);
			}
		}

		WindowAPI* window = nullptr;

		KeyboardState OldKS;
		KeyboardState NewKS;
		MouseState OldMS;
		MouseState NewMS;

		std::unordered_map<std::string, InputEvent> events;

	};

}
