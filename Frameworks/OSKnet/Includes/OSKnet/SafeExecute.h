#pragma once

#include <enet/enet.h>

#include "Message.h"
#include "Callbacks.h"
#include "Macros.h"

namespace OSK::NET {

	//INTERNAL.
	//Ejecuta el callback sólamente si no es nullptr.
	template <typename T> void OSKnet_safe_callback_execute(T callback, Message& msg) {
		if (callback != nullptr)
			callback(msg);
	}

}