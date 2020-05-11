#include "Server.h"

namespace OSK::NET {

	void Server::Setup(const uint32_t& ip, const uint32_t& port) {
		address.host = ip;
		address.port = port;
	}


	void Server::SetNewConnectionCallback(OSKnet_new_connection_callback callback) {
		new_connection_callback = callback;
	}


	void Server::SetMessageReceivedCallback(OSKnet_message_received_callback callback) {
		message_received_callback = callback;
	}


	void Server::SetDisconnectCallback(OSKnet_disconnect_callback callback) {
		disconnection_callback = callback;
	}


	bool Server::Start(const uint32_t& max_connections, const uint32_t& channel_count, const uint32_t& in_bandwidht, const uint32_t& out_bandwidht) {
		if (is_running)
			return false;

		server = enet_host_create(&address, max_connections, channel_count, in_bandwidht, out_bandwidht);

		is_running = server != nullptr;

		return is_running;
	}


	void Server::ProccessMessages() {
		while (enet_host_service(server, &message, 0) > 0) {
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


	/*void Server::SendMessageToAll(Packet* message, const uint32_t& channel) {
		std::string buffer = (char*)message->ID;
		buffer.append(message->Data);
		std::cout << "ENCODE: " << message->Data << " -> " << buffer.c_str() << std::endl;
		ENetPacket* packet = enet_packet_create(buffer.c_str(), buffer.length(), 0);
		enet_host_broadcast(server, 0, packet);

		outgoing_messages++;
	}*/


	void Server::SendMessageToAll(const char* message, const uint32_t& channel) {
		ENetPacket* packet = enet_packet_create(message, strlen(message) + 1, 0);
		enet_host_broadcast(server, 0, packet);

		outgoing_messages++;
	}


	void Server::Flush() {
		if (outgoing_messages > 0)
			enet_host_flush(server);

		outgoing_messages = 0;
	}


	void Server::Close() {
		if (!is_running)
			return;

		enet_host_destroy(server);

		is_running = false;
	}

}