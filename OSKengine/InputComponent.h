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

	class OSKAPI_CALL InputComponent : public Component {
				
	public:

		std::function<void()>& GetFunction(const std::string eventName) {
			if (inputEvents.find(eventName) == inputEvents.end())
				inputEvents.insert({ eventName, []() {} });

			return inputEvents[eventName];
		}

		void ExecuteFunction(const std::string& eventName) {
			if (inputEvents.find(eventName) != inputEvents.end()) {
				GetFunction(eventName)();
			}
		}

	private:

		std::unordered_map<std::string, std::function<void()>> inputEvents;

	};

}
