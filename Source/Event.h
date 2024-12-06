#pragma once

#include <string_view>

#ifndef OSK_EVENT
#define OSK_EVENT(className) static inline std::string_view GetEventName() { return className; }
#endif

namespace OSK::ECS {

	/// @brief Concepto que nos permite detectar si una clase cumple con las condiciones
	/// para ser un evento.
	template<typename TEvent>
	concept IsEcsEvent = requires (TEvent) {
		{ TEvent::GetEventName() };
	};

}
