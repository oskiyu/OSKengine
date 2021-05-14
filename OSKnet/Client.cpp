#include "Client.h"

#include <string>

namespace OSK::NET {

	void Client::Setup(const uint32_t& channel_count , const uint32_t& in_bandwidht , const uint32_t& out_bandwidht ) {
		client = enet_host_create(nullptr, 1, channel_count, in_bandwidht, out_bandwidht);
		numero_de_canales = channel_count;
	}


	bool Client::Connect(const char* ip, const uint32_t& port) {
		enet_address_set_host(&address, ip);
		address.port = port;
		connected_server = enet_host_connect(client, &address, numero_de_canales, 0);

		is_connected = connected_server != nullptr;

		return is_connected;
	}


	void Client::ProccessMessages() {
		while (enet_host_service(client, &message, 0) > 0) {

			Message oskMsg(message);

			switch (message.type) {

			case ENET_EVENT_TYPE_CONNECT:
				NewConnectionCallback(oskMsg);
				break;

			case ENET_EVENT_TYPE_RECEIVE: {
				MessageReceivedCallback(oskMsg);

				//Destruir el mensaje una vez ha sido inspeccionado.
				enet_packet_destroy(message.packet);
			}
				break;

			case ENET_EVENT_TYPE_DISCONNECT: {
				DisconnectCallback(oskMsg);

				message.peer->data = nullptr;
			}
				break;

			}
		}
	}


	void Client::SendMessage(Message& message, const uint32_t& channel) {
		ENetPacket* packet = enet_packet_create(message.buffer.Read(0), message.buffer.GetSize() + 1, 0);
		enet_peer_send(connected_server, channel, packet);
	}


	void Client::Flush() {
		enet_host_flush(client);
	}


	void Client::Disconnect() {
		if (!is_connected)
			return;

		enet_host_destroy(client);
		enet_peer_disconnect(connected_server, 0);

		is_connected = false;
	}

}