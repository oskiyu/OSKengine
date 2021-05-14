#include "Server.h"

namespace OSK::NET {

	void Server::Setup(const uint32_t& ip, const uint32_t& port) {
		address.host = ip;
		address.port = port;

		id = 0;
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

			Message oskMsg{ message };

			switch (message.type) {

				case ENET_EVENT_TYPE_CONNECT:
					OSKnet_safe_callback_execute<>(new_connection_callback, oskMsg);
				
					client_id_map[message.peer] = id++;
					id_client_map[id] = message.peer;

					break;

				case ENET_EVENT_TYPE_RECEIVE: {
					OSKnet_safe_callback_execute<>(message_received_callback, oskMsg);

					//Destruir el mensaje una vez ha sido inspeccionado.
					enet_packet_destroy(message.packet);
					}
					break;

				case ENET_EVENT_TYPE_DISCONNECT: {
					OSKnet_safe_callback_execute<>(disconnection_callback, oskMsg);

					id_client_map.erase(GetID(message.peer));
					client_id_map.erase(message.peer);

					message.peer->data = nullptr;
					}
					break;

			}
		}
	}


	void Server::SendMessageToAll(Message& message, const uint32_t& channel) {
		ENetPacket* packet = enet_packet_create(message.buffer.Read(0), message.buffer.GetSize() + 1, 0);
		enet_host_broadcast(server, channel, packet);
	}


	void Server::SendMessageToClient(Message& message, ENetPeer* client, const uint32_t& channel) {
		ENetPacket* packet = enet_packet_create(message.buffer.Read(0), message.buffer.GetSize() + 1, 0);
		enet_peer_send(client, channel, packet);
	}


	void Server::Flush() {
		enet_host_flush(server);
	}


	void Server::Close() {
		if (!is_running)
			return;

		enet_host_destroy(server);

		is_running = false;
	}

}