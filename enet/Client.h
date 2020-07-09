#pragma once

#include <enet/enet.h>

#include <iostream>

#include "Macros.h"
#include "Callbacks.h"
#include "SafeExecute.h"

#undef SendMessage

namespace OSK::NET {

	//representa un cliente que puede conectarse a un servidor.
	class OSKAPI_CALL Client {

	public:

		//Prepara el cliente para poder ser utilizado.
		//-channerl_count: número de canales.
		//-in_bandwidth: límite de ancho de banda de entrada.
		//-out_bandwidth: límite de ancho de banda de salida.
		void Setup(const uint32_t& channel_count = 2, const uint32_t& in_bandwidht = 0, const uint32_t& out_bandwidht = 0);


		//Asigna la función que se ejecutará al conectarse con el servidor.
		//-callback: Función que se ejecutará.
		void SetNewConnectionCallback(OSKnet_new_connection_callback callback);


		//Asigna la función que se ejecutará al recibir un mensaje del servidor.
		//-callback: Función que se ejecutará.
		void SetMessageReceivedCallback(OSKnet_message_received_callback callback);


		//Asigna la función que se ejecutará al recibir un mensaje de desconexión.
		//-callback: Función que se ejecutará.
		void SetDisconnectCallback(OSKnet_disconnect_callback callback);


		//Conecta el cliente con un servidor.
		//-ip = ip del servidor.
		//-port = puerto del servidor.
		bool Connect(const char* ip, const uint32_t& port);


		//Procesa los mensajes.
		void ProccessMessages();


		//Envía un mensaje al servidor.
		//El mensaje se introduce en una cola para ser enviado.
		//-message: mensaje a enviar.
		//-channel: canal por el que se va a enviar el mensaje.
		void SendMessage(Message& message, const uint32_t& channel = 0);


		//Envía todos los mensajes que están en la cola para ser enviados.
		void Flush();


		//Desconecta el cliente del servidor.
		void Disconnect();

	private:

		//Dirección del server.
		ENetAddress address = {};

		//Cliente de ENET.
		ENetHost* client = nullptr;

		//Servidor al que está conectado
		ENetPeer* connected_server = nullptr;

		//Mensaje (para procesar los mensajes que llegan).
		ENetEvent message = {};

		//Función que se va a ejecutar al desconectarse.
		OSKnet_disconnect_callback disconnection_callback = nullptr;

		//Función que se va a ejecutar al conectarse.
		OSKnet_new_connection_callback new_connection_callback = nullptr;

		//Función que se va a ejecutar al recibir un mensaje.
		OSKnet_message_received_callback message_received_callback = nullptr;

		//Número de canales.
		unsigned int numero_de_canales = 0;

		//Para procesar los mensajes.
		int message_status = 0;

		//Comprueba si está conectado.
		bool is_connected = false;

	};

}