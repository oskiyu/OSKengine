#pragma once

#include <enet/enet.h>

#include <iostream>

#include "Callbacks.h"
#include "SafeExecute.h"

#include <map>
#include "Types.h"
#include "Macros.h"

namespace OSK::NET {

	constexpr uint32_t SERVER_IP_ANY = 0;


	//Representa un servidor.
	//Otros clientes se pueden unir a este servidor.
	//El servidor almacena una lista con los clientes conectados.
	class OSKAPI_CALL Server {

	public:

		//Prepara el servidor para su uso.
		//-ip = ip del servidor (OSK::NET::SERVER_IP_ANY para IP automática).
		//-port = puerto del servidor.
		void Setup(const uint32_t& ip, const uint32_t& port);


		//Asigna la función que se ejecutará al conectarse un cliente.
		//-callback: Función que se ejecutará.
		void SetNewConnectionCallback(OSKnet_new_connection_callback callback);


		//Asigna la función que se ejecutará al recibir el mensaje de un cliente.
		//-callback: Función que se ejecutará.
		void SetMessageReceivedCallback(OSKnet_message_received_callback callback);


		//Asigna la función que se ejecutará al desconectarse un cliente.
		//-callback: Función que se ejecutará.
		void SetDisconnectCallback(OSKnet_disconnect_callback callback);


		//Inicia el servidor.
		//-max_connections: Número máximo de conexiones permitidas.
		//-channerl_count: número de canales.
		//-in_bandwidth: límite de ancho de banda de entrada.
		//-out_bandwidth: límite de ancho de banda de salida.
		bool Start(const uint32_t& max_connections, const uint32_t& channel_count = 2, const uint32_t& in_bandwidht = 0, const uint32_t& out_bandwidht = 0);


		//Procesa los mensajes.
		void ProccessMessages();


		//Envía un mensaje a todos los clientes conectados.
		//Los mensajes se introducen en una cola para ser enviados.
		//-message: mensaje a enviar.
		//-channel: canal por el que se va a enviar el mensaje.
		void SendMessageToAll(Message&, const uint32_t& channel = 0);


		//Envía un mensaje a un cliente en particular.
		//El mensaje se introduce en una cola para ser enviado.
		//-message: mensaje a enviar.
		//-clientID: ID del cliente al que se le va a enviar el mensaje.
		//-channel: canal por el que se va a enviar el mensaje.
		inline void SendMessageToClient(Message& message, const clientID_t& clientID, const uint32_t& channel = 0) {
			SendMessageToClient(message, GetClient(clientID), channel);
		}


		//Envía un mensaje a un cliente en particular.
		//El mensaje se introduce en una cola para ser enviado.
		//-message: mensaje a enviar.
		//-client: cliente al que se le va a enviar el mensaje.
		//-channel: canal por el que se va a enviar el mensaje.
		void SendMessageToClient(Message&, ENetPeer* client, const uint32_t& channel = 0);

		
		//Envía todos los mensajes que están en la cola para ser enviados.
		void Flush();


		//Cierra el servidor.
		void Close();


		//Devuelve el ID del cliente conectado especificado.
		//-client: cliente del que se quiere conocer su ID.
		inline clientID_t GetID(ENetPeer* client) {
			return client_id_map.at(client);
		}


		//Devuelve el cliente conectado con la id especificada.
		//-ID: ID del cliente.
		inline ENetPeer* GetClient(const clientID_t& ID) {
			return id_client_map.at(ID);
		}

	private:

		//Dirección del server.
		ENetAddress address = {};

		//Servidor de ENET.
		ENetHost* server = nullptr;

		//Mensaje (para procesar los mensajes que llegan).
		ENetEvent message = {};

		//Función que se va a ejecutar al desconectarse un cliente.
		OSKnet_disconnect_callback disconnection_callback = nullptr;

		//Función que se va a ejecutar al conectarse un cliente.
		OSKnet_new_connection_callback new_connection_callback = nullptr;

		//Función que se va a ejecutar al recibir un mensaje.
		OSKnet_message_received_callback message_received_callback = nullptr;

		//Para procesar los mensajes.
		int message_status = 0;

		//Comprueba la ejecución del server.
		bool is_running = false;

		//ID que se va a otorgar al próximo cliente que se conecte.
		clientID_t id;

		//Diccionarios de cliente-ID e ID-cliente.
		std::map<ENetPeer*, clientID_t> client_id_map{};
		std::map<clientID_t, ENetPeer*> id_client_map{};

	};

}