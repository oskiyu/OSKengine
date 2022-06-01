module;

#include <functional>
#include <OSKengine/KeyboardState.h>
#include <OSKengine/MouseState.h>
#include <OSKengine/HashMap.hpp>
#include <OSKengine/LinkedList.hpp>
#include <OSKengine/UniquePtr.hpp>
#include <OSKengine/Logger.h>
#include <OSKengine/OSKengine.h>
#include <OSKengine/Window.h>

export module InputEventRaiser;

using namespace OSK;
using namespace OSK::IO;

export using InputEventId = TSize;

export class InputEventRaiser {

public:

	inline void RegisterKeyPressedInput(Key key, const std::function<void(float)>& callback) {
		keyCallbacks.Insert({ key, callback });
	}

	inline void RegisterKeyStrokeInput(Key key, const std::function<void(float)>& callback) {
		keyStrokeCallbacks.Insert({ key, callback });
	}


	void RaiseEvents(float deltaTime) {
		for (TSize i = 0; i < keyCallbacks.GetSize(); i++)
			if (Engine::GetWindow()->GetKeyboardState()->IsKeyDown(keyCallbacks[i].first))
				keyCallbacks[i].second(deltaTime);

		for (TSize i = 0; i < keyStrokeCallbacks.GetSize(); i++)
			if (Engine::GetWindow()->GetKeyboardState()->IsKeyDown(keyStrokeCallbacks[i].first) && Engine::GetWindow()->GetPreviousKeyboardState()->IsKeyUp(keyStrokeCallbacks[i].first))
				keyStrokeCallbacks[i].second(deltaTime);
	}

private:

	LinkedList<Pair<Key, std::function<void(float)>>> keyCallbacks;
	LinkedList<Pair<Key, std::function<void(float)>>> keyStrokeCallbacks;

};
