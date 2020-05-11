#include "Client.h"

#include <string>

namespace OSK::NET {

	void Client::Setup(const uint32_t& channel_count , const uint32_t& in_bandwidht , const uint32_t& out_bandwidht ) {
		client = enet_host_create(nullptr, 1, channel_count, in_bandwidht, out_bandwidht);
		numero_de_canales = channel_count;
	}


	void Client::SetNewConnectionCallback(OSKnet_new_connection_callback callback) {
		new_connection_callback = callback;
	}


	void Client::SetMessageReceivedCallback(OSKnet_message_received_callback callback) {
		message_received_callback = callback;
	}


	void Client::SetDisconnectCallback(OSKnet_disconnect_callback callback) {
		disconnection_callback = callback;
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
			std::cout << "MSG" << std::endl;
			switch (message.type) {

			case ENET_EVENT_TYPE_CONNECT:
				OSKnet_safe_callback_execute<>(new_connection_callback, message);
				break;

			case ENET_EVENT_TYPE_RECEIVE: {
				OSKnet_safe_callback_execute<>(message_received_callback, message);

				//Destruir el mensaje una vez ha sido inspeccionado.
				enet_packet_destroy(message.packet);
			}
				break;

			case ENET_EVENT_TYPE_DISCONNECT: {
				OSKnet_safe_callback_execute<>(disconnection_callback, message);

				message.peer->data = nullptr;
			}
				break;

			}
		}
	}


	void Client::SendMessage(const char* message, const uint32_t& channel) {
		ENetPacket* packet = enet_packet_create(message, strlen(message) + 1, 0);
		enet_peer_send(connected_server, channel, packet);

		outgoing_messages++;
	}


	void Client::Flush() {
		if (outgoing_messages > 0)
			enet_host_flush(client);

		outgoing_messages = 0;
	}


	void Client::Disconnect() {
		if (!is_connected)
			return;

		enet_host_destroy(client);
		enet_peer_disconnect(connected_server, 0);

		is_connected = false;
	}

}