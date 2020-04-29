#pragma once

#include <enet/enet.h>

#include <iostream>

//#include "MessageType.h"

namespace OSK::NET {

	typedef void (*NewConnectionCallback) (ENetEvent& message);

	typedef void (*MessageReceivedCallback) (ENetEvent& message);

	typedef void (*DisconnectionCallback) (ENetEvent& message);

	template <class T>T SafeExecute(T callback) {
		if (callback != nullptr)
			callback();
	}

	constexpr int SERVER_IP_ANY = 0;

	class Server {

	public:

		void Setup(unsigned int ip, unsigned int port) {
			address.host = ip;
			address.port = port;
		}

		bool Start(unsigned int max_connections, unsigned int channel_count = 2, unsigned int in_bandwidht = 0, unsigned int out_bandwidht = 0) {
			if (is_running)
				return;

			server = enet_host_create(&address, max_connections, channel_count, in_bandwidht, out_bandwidht);

			is_running = server != nullptr;

			return is_running;
		}

		void ProccessMessages() {
			message_status = enet_host_service(server, &message, 0);

			while (message_status > 0) {

				switch (message.type) {

				case ENET_EVENT_TYPE_CONNECT:
					SafeExecute<>(new_connection_callback);
					break;

				case ENET_EVENT_TYPE_RECEIVE:
					SafeExecute<>(message_received_callback);

					//Destruir el mensaje una vez ha sido inspeccionado.
					enet_packet_destroy(message.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					SafeExecute<>(disconnection_callback);

					message.peer->data = nullptr;
					break;

				}
			}
		}

		void SendMessageToAll(void* msg, unsigned int channel = 0) {
			ENetPacket* packet = enet_packet_create(msg, sizeof(msg), 0);
			enet_host_broadcast(server, 0, packet);

			outgoing_messages++;
		}

		void Flush() {
			if (outgoing_messages > 0)
				enet_host_flush(server);
		}

		void Close() {
			if (!is_running)
				return;

			enet_host_destroy(server);

			is_running = false;
		}

	private:

		ENetAddress address;

		ENetHost* server = nullptr;

		ENetEvent message;

		DisconnectionCallback disconnection_callback = nullptr;

		NewConnectionCallback new_connection_callback = nullptr;

		MessageReceivedCallback message_received_callback = nullptr;

		int message_status = 0;

		int outgoing_messages = 0;

		bool is_running = false;

	};

}