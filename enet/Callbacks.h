#pragma once

#include <enet/enet.h>

#include "Packet.h"

namespace OSK::NET {

	typedef void (*OSKnet_message_callback) (const ENetEvent& message);

	typedef OSKnet_message_callback OSKnet_disconnect_callback;

	typedef OSKnet_message_callback OSKnet_message_received_callback;

	typedef void (*OSKnet_new_connection_callback) (const ENetEvent& msg);

}