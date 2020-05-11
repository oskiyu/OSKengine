#pragma once

#include <enet/enet.h>

#include <iostream>

#include "Callbacks.h"
#include "SafeExecute.h"

#include "Packet.h"
#include "Encoder.hpp"

#undef SendMessage

namespace OSK::NET {

	class Client {

	public:

		void Setup(const uint32_t& channel_count = 2, const uint32_t& in_bandwidht = 0, const uint32_t& out_bandwidht = 0);

		void SetNewConnectionCallback(OSKnet_new_connection_callback callback);

		void SetMessageReceivedCallback(OSKnet_message_received_callback callback);

		void SetDisconnectCallback(OSKnet_disconnect_callback callback);

		bool Connect(const char* ip, const uint32_t& port);

		void ProccessMessages();

		void SendMessage(const char* message, const uint32_t& channel = 0);

		void Flush();

		void Disconnect();

	private:

		ENetAddress address = {};

		ENetHost* client = nullptr;

		ENetPeer* connected_server = nullptr;

		ENetEvent message = {};

		OSKnet_disconnect_callback disconnection_callback = nullptr;

		OSKnet_new_connection_callback new_connection_callback = nullptr;

		OSKnet_message_received_callback message_received_callback = nullptr;

		unsigned int numero_de_canales = 0;

		int message_status = 0;

		int outgoing_messages = 0;

		bool is_connected = false;

	};

}