# OSK::NET

## Documentación de enet.


### Crear un servidor.

```cpp

ENetAddress address;
ENetHost* server;

/***/

address.host = ENET_HOST_ANY;
address.port = 1234; //Puerto.

//Crea el servidor.
//	-Address.
//	-Número máximo de conexiones.
//	-Número de canales.
//	-Límite de ancho de banda de entrada (0 = cualquiera).
//	-Límite de ancho de banda de salida (0 = cualquiera).

server = enet_host_create(&address, 32, 2, 0, 0);
```


### Procesar mensajes recibidos.

```cpp

ENetHost* server;
ENetEvent message;

/***/

//Obtener nuevos mensajes.
//	-Servidor.
//	-Evento (mensaje).
//	-Timeout.
msg_status = enet_host_service(server, &message, 50000);

while (msg_status > 0) {

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

```


### Enviar mensaje desde el servidor a todos los clientes.

```cpp

ENetHost* server;

/***/

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

```


### Enviar mensaje a una conexión en particluar.

```cpp

ENetPeer* peer = nullptr;

/***/

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

```


### Desconectarse de un servidor.

```cpp

ENetPeer* server = nullptr;

/***/

//Desconexión.
//	-Conexión de la que desconectarse.
//	-Mensaje de despedida.
enet_peer_disconnect(server, 0);

```


### Crear un cliente.

```cpp

ENetHost* client = nullptr;

/***/

//Crea el cliente.
//	-.
//	-Conexiones salientes.
//	-Número de canales.
//	-Límite de ancho de banda de entrada (0 = cualquiera).
//	-Límite de ancho de banda de salida (0 = cualquiera).
client = enet_host_create(nullptr, 1, 2, 0, 0);

```


### Conectarse a un servidor.

```cpp

ENetHost* client = nullptr;
ENetPeer* connected_server = nullptr;
ENetEvent message;

//Dirección para conectarse.
ENetAddress address;

/***/

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
connected_server = enet_host_connect(client, &address, 2, 0);

//Error-handling.
if (connected_server == nullptr) {
	//DO SOMETHING.
}

//Esperar para confirmar la conexión (5 segundos).
//See: Recibir un mensaje.
msg_status = enet_host_service(client, &message, 5000);
if (msg_status > 0 && message.type == ENET_EVENT_TYPE_CONNECT) {
	//DO: SUCCESS.
}
else {
	enet_peer_reset(connected_server);
	
	//DO: FAILURE.
}

```