#pragma once

#include <enet/enet.h>

#include <iostream>

namespace OSK::NET {

	class Server {

	public:

		void Setup(unsigned int ip, unsigned int port) {
			address.host = ip;
			address.port = port;
		}

		bool Start(unsigned int max_connections, unsigned int channel_count = 2, unsigned int in_bandwidht = 0, unsigned int out_bandwidht = 0) {
			server = enet_host_create(&address, max_connections, channel_count, in_bandwidht, out_bandwidht);

			if (server == nullptr)
				return false;

			return true;
		}

		void ProccessMessages() {
			message_status = enet_host_service(server, &message, 50000);

			while (message_status > 0) {

				switch (message.type) {

				case ENET_EVENT_TYPE_CONNECT:
					std::cout << "NEW CONNECTION: " << message.peer->address.host << std::endl;
					break;

				case ENET_EVENT_TYPE_RECEIVE:
					std::cout << "MESSAGE FROM " << message.peer->address.host << ": " << message.peer->data << std::endl;

					//Destruir el mensaje una vez ha sido inspeccionado.
					enet_packet_destroy(message.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << "DISSCONNECT: " << message.peer->address.host << std::endl;
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

	private:

		ENetAddress address;

		ENetHost* server = nullptr;

		ENetEvent message;

		int message_status = 0;

		int outgoing_messages = 0;

	};

}