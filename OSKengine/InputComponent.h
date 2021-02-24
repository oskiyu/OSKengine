#pragma once

#include "ECS.h"

#include <functional>

#include "KeyboardState.h"
#include "MouseState.h"

namespace OSK {

	struct OSKAPI_CALL InputEvent {

		std::string EventName = "";

		std::vector<Key> LinkedKeys;
		std::vector<ButtonCode> LinkedButtons;

	};

	struct OSKAPI_CALL OneTimeInputEvent {

		std::string EventName = "";

		std::vector<Key> LinkedKeys;
		std::vector<ButtonCode> LinkedButtons;

	};

	class OSKAPI_CALL InputComponent : public Component {
				
	public:

		std::function<void(deltaTime_t)>& GetInputFunction(const std::string eventName) {
			if (inputEvents.find(eventName) == inputEvents.end())
				inputEvents.insert({ eventName, [](deltaTime_t deltaTime) {} });

			return inputEvents[eventName];
		}
		std::function<void()>& GetOneTimeInputFunction(const std::string eventName) {
			if (oneTimeInputEvents.find(eventName) == oneTimeInputEvents.end())
				oneTimeInputEvents.insert({ eventName, []() {} });

			return oneTimeInputEvents[eventName];
		}

		void ExecuteInputFunction(const std::string& eventName, deltaTime_t deltaTime) {
			if (inputEvents.find(eventName) != inputEvents.end()) {
				GetInputFunction(eventName)(deltaTime);
			}
		}
		void ExecuteOneTimeInputFunction(const std::string& eventName) {
			if (oneTimeInputEvents.find(eventName) != oneTimeInputEvents.end()) {
				GetOneTimeInputFunction(eventName)();
			}
		}

	private:

		std::unordered_map<std::string, std::function<void(deltaTime_t)>> inputEvents;
		std::unordered_map<std::string, std::function<void()>> oneTimeInputEvents;

	};

}
