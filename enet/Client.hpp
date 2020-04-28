#pragma once

#include <enet/enet.h>

#include <iostream>

#undef SendMessage

namespace OSK::NET {

	class Client {

	public:

		void Setup(unsigned int channel_count = 2, unsigned int in_bandwidht = 0, unsigned int out_bandwidht = 0) {
			client = enet_host_create(nullptr, 1, channel_count, in_bandwidht, out_bandwidht);
			numero_de_canales = channel_count;
		}

		bool Connect(char* ip, unsigned int port) {
			enet_address_set_host(&address, ip);
			address.port = port;
			connected_server = enet_host_connect(client, &address, numero_de_canales, 0);
		}

		void ProccessMessages() {
			message_status = enet_host_service(client, &message, 50000);

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

		void SendMessage(void* msg, unsigned int channel = 0) {
			ENetPacket* packet = enet_packet_create(msg, sizeof(msg), 0);
			enet_peer_send(connected_server, channel, packet);

			outgoing_messages++;
		}

		void Flush() {
			if (outgoing_messages > 0)
				enet_host_flush(client);
		}

	private:

		ENetAddress address;

		ENetHost* client = nullptr;

		ENetPeer* connected_server = nullptr;

		ENetEvent message;

		unsigned int numero_de_canales = 0;

		int message_status = 0;

		int outgoing_messages = 0;

	};

}