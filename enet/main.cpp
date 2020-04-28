#include <enet/enet.h>

#include <iostream>

#define SERVER
#define CLIENT

#ifdef SERVER

int main(int argc, char** argv) {
	ENetAddress address;
	ENetHost* server;
	ENetEvent event;

	//Ejemplo.
	ENetHost* client = nullptr;
	ENetPeer* peer = nullptr;

	int event_status;

	int result = enet_initialize();
	if (result != 0) {
		std::cout << "ERROR: inicializar enet" << std::endl;
		
		return -1;
	}

	//Call enet_deinitialize() al salir.
	atexit(enet_deinitialize);

	//IP automática.
	address.host = ENET_HOST_ANY;
	address.port = 1234;

	//Crea el servidor.
	//	-Address.
	//	-Número máximo de conexiones.
	//	-Número de canales.
	//	-Límite de ancho de banda de entrada (0 = cualquiera).
	//	-Límite de ancho de banda de salida (0 = cualquiera).
	server = enet_host_create(&address, 32, 2, 0, 0);

	if (server == nullptr) {
		std::cout << "ERROR: crear servidor" << std::endl;

		return -1;
	}

	while (true) {
		
		//Obtener nuevos mensajes.
		//	-Servidor.
		//	-Evento (mensaje).
		//	-Timeout.
		event_status = enet_host_service(server, &event, 50000);

		while (event_status > 0) {

			switch (event.type) {
			
				case ENET_EVENT_TYPE_CONNECT:
					std::cout << "NEW CONNECTION: " << event.peer->address.host << std::endl;
					break;

				case ENET_EVENT_TYPE_RECEIVE:
					std::cout << "MESSAGE FROM " << event.peer->address.host << ": " << event.peer->data << std::endl;
					
					//Destruir el mensaje una vez ha sido inspeccionado.
					enet_packet_destroy(event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << "DISSCONNECT: " << event.peer->address.host << std::endl;
					event.peer->data = nullptr;
					break;

			}
		}
	}

	//Enviar mensaje desde el servidor a todos los clientes.
	{
		//Crea el mensaje.
		//	-Data.
		//	-Data size.
		//	-Flags.
		ENetPacket* packet = enet_packet_create("mensaje", strlen("mensaje") + 1, ENET_PACKET_FLAG_RELIABLE);

		//Enviar el mensaje a todos los clientes de un servidor.
		//	-Server.
		//	-Canal.
		//	-Mensaje.
		enet_host_broadcast(server, 0, packet);

		//Envia todos los mensajes del servidor.
		//	-Server.
		enet_host_flush(server);
	}

	//Enviar mensaje a una conexión en particluar.
	{
		//Crea el mensaje.
		//	-Data.
		//	-Data size.
		//	-Flags.
		ENetPacket* packet = enet_packet_create("mensaje", strlen("mensaje") + 1, ENET_PACKET_FLAG_RELIABLE);

		//Envia el mensaje a una conexión particular.
		//	-Conexión.
		//	-Canal.
		//	-Mensaje.
		enet_peer_send(peer, 0, packet);

		//Envia todos los mensajes del servidor.
		//	-Server.
		enet_host_flush(server);
	}

	//Desconectarse de un servidor.
	{
		//Desconexión.
		//	-Conexión de la que desconectarse.
		//	-Mensaje de despedida.
		enet_peer_disconnect(peer, 0);
	}

	//Crear un cliente.
	{
		//Crea el cliente.
		//	-.
		//	-Conexiones salientes.
		//	-Número de canales.
		//	-Límite de ancho de banda de entrada (0 = cualquiera).
		//	-Límite de ancho de banda de salida (0 = cualquiera).
		client = enet_host_create(nullptr, 1, 2, 0, 0);
	}

	//Conectarse a un servidor.
	{
		//Dirección para conectarse.
		ENetAddress address;

		//Establece la dirección a la que conectarse.
		//	-ENetAddress.
		//	-Dirección.
		enet_address_set_host(&address, "localhost");

		//Puerto.
		address.port = 1234;

		//Conexión.
		//	-Cliente.
		//	-ENetAddress.
		//	-Número de canales.
		//	-Mensaje de conexión.
		peer = enet_host_connect(client, &address, 2, 0);

		//Error-handling.
		if (peer == nullptr) {
			//DO SOMETHING.
		}

		//Esperar para confirmar la conexión (5 segundos).
		//See: Recibir un mensaje.
		event_status = enet_host_service(client, &event, 5000);
		if (event_status > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
			//DO: SUCCESS.
		}
		else {
			enet_peer_reset(peer);

			//DO: FAILURE.
		}
	}
}

#endif

#ifdef CLIENT

#include <string>

int main(int argc, char** argv) {
	ENetAddress address;
	ENetHost* client = nullptr;
	ENetPeer* peer = nullptr;
	char message[1024];
	ENetEvent event;
	int event_status;

	int result = enet_initialize();
	if (result != 0) {
		std::cout << "ERROR: iniciar enet" << std::endl;
		return -1;
	}

	//Call enet_deinitialize() al salir.
	atexit(enet_deinitialize);

	client = enet_host_create(NULL, 1, 2, 57600 / 8, 14400 / 8);

	if (client == nullptr) {
		std::cout << "ERROR: iniciar cliente" << std::endl;
		return -1;
	}

	enet_address_set_host(&address, "localhost");
	address.port = 1234;

	peer = enet_host_connect(client, &address, 2, 0);

	if (peer == nullptr) {
		std::cout << "ERROR: no hay servidor al que conectarse" << std::endl;
		return -1;
	}

	event_status = 1;

}

#endif

#ifdef DOCUMENTATION



#endif