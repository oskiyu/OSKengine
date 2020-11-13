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
		//-channerl_count: n�mero de canales.
		//-in_bandwidth: l�mite de ancho de banda de entrada.
		//-out_bandwidth: l�mite de ancho de banda de salida.
		void Setup(const uint32_t& channel_count = 2, const uint32_t& in_bandwidht = 0, const uint32_t& out_bandwidht = 0);


		//Asigna la funci�n que se ejecutar� al conectarse con el servidor.
		//-callback: Funci�n que se ejecutar�.
		void SetNewConnectionCallback(OSKnet_new_connection_callback callback);


		//Asigna la funci�n que se ejecutar� al recibir un mensaje del servidor.
		//-callback: Funci�n que se ejecutar�.
		void SetMessageReceivedCallback(OSKnet_message_received_callback callback);


		//Asigna la funci�n que se ejecutar� al recibir un mensaje de desconexi�n.
		//-callback: Funci�n que se ejecutar�.
		void SetDisconnectCallback(OSKnet_disconnect_callback callback);


		//Conecta el cliente con un servidor.
		//-ip = ip del servidor.
		//-port = puerto del servidor.
		bool Connect(const char* ip, const uint32_t& port);


		//Procesa los mensajes.
		void ProccessMessages();


		//Env�a un mensaje al servidor.
		//El mensaje se introduce en una cola para ser enviado.
		//-message: mensaje a enviar.
		//-channel: canal por el que se va a enviar el mensaje.
		void SendMessage(Message& message, const uint32_t& channel = 0);


		//Env�a todos los mensajes que est�n en la cola para ser enviados.
		void Flush();


		//Desconecta el cliente del servidor.
		void Disconnect();

	private:

		//Direcci�n del server.
		ENetAddress address = {};

		//Cliente de ENET.
		ENetHost* client = nullptr;

		//Servidor al que est� conectado
		ENetPeer* connected_server = nullptr;

		//Mensaje (para procesar los mensajes que llegan).
		ENetEvent message = {};

		//Funci�n que se va a ejecutar al desconectarse.
		OSKnet_disconnect_callback disconnection_callback = nullptr;

		//Funci�n que se va a ejecutar al conectarse.
		OSKnet_new_connection_callback new_connection_callback = nullptr;

		//Funci�n que se va a ejecutar al recibir un mensaje.
		OSKnet_message_received_callback message_received_callback = nullptr;

		//N�mero de canales.
		unsigned int numero_de_canales = 0;

		//Para procesar los mensajes.
		int message_status = 0;

		//Comprueba si est� conectado.
		bool is_connected = false;

	};

}