#pragma once

#include <enet/enet.h>

#include <iostream>

#include "Callbacks.h"
#include "SafeExecute.h"

#include "Packet.h"
#include "Encoder.hpp"

namespace OSK::NET {

	constexpr uint32_t SERVER_IP_ANY = 0;

	class Server {

	public:

		void Setup(const uint32_t& ip, const uint32_t& port);

		void SetNewConnectionCallback(OSKnet_new_connection_callback callback);

		void SetMessageReceivedCallback(OSKnet_message_received_callback callback);

		void SetDisconnectCallback(OSKnet_disconnect_callback callback);

		bool Start(const uint32_t& max_connections, const uint32_t& channel_count = 2, const uint32_t& in_bandwidht = 0, const uint32_t& out_bandwidht = 0);

		void ProccessMessages();

		void SendMessageToAll(const char* message, const uint32_t& channel = 0);

		void Flush();

		void Close();

	private:

		ENetAddress address = {};

		ENetHost* server = nullptr;

		ENetEvent message = {};

		OSKnet_disconnect_callback disconnection_callback = nullptr;

		OSKnet_new_connection_callback new_connection_callback = nullptr;

		OSKnet_message_received_callback message_received_callback = nullptr;

		int message_status = 0;

		uint32_t outgoing_messages = 0;

		bool is_running = false;

	};

}