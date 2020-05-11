#pragma once

#include <enet/enet.h>

#include "Packet.h"
#include "Callbacks.h"

namespace OSK::NET {

	template <typename T> void OSKnet_safe_callback_execute(T callback, const ENetEvent& msg) {
		if (callback != nullptr)
			callback(msg);
	}

}