#pragma once

#include "ECS.h"
#include "InputComponent.h"

#include <string>
#include "WindowAPI.h"

namespace OSK {

	class InputSystem : public ECS::System {

	public:

		void SetWindow(WindowAPI* window);

		void OnTick(deltaTime_t deltaTime) override;
		Signature GetSystemSignature() override;

		inline InputEvent& GetInputEvent(const std::string& e) {
			return events[e];
		}

		inline void RegisterInputEvent(const std::string& name) {
			events.insert({ name, InputEvent{ name } });
		}

		inline OneTimeInputEvent& GetOneTimeInputEvent(const std::string& e) {
			return oneTimeEvents[e];
		}

		inline void RegisterOneTimeInputEvent(const std::string& name) {
			oneTimeEvents.insert({ name, OneTimeInputEvent{ name } });
		}

	private:

		void executeFunction(std::string& name, deltaTime_t deltaTime);

		void executeOneTimeFunction(std::string& name);

		WindowAPI* window = nullptr;

		KeyboardState OldKS;
		KeyboardState NewKS;
		MouseState OldMS;
		MouseState NewMS;

		std::unordered_map<std::string, InputEvent> events;
		std::unordered_map<std::string, OneTimeInputEvent> oneTimeEvents;

	};

}
