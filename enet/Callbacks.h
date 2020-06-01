#pragma once

#include <enet/enet.h>

#include "Message.hpp"

namespace OSK::NET {

	//Callback general para procesar mensajes.
	typedef void (*OSKnet_message_callback) (Message& message);


	//Callback para procesar desconexiones.
	//-void nombre(const ENetEvent& message).
	typedef OSKnet_message_callback OSKnet_disconnect_callback;


	//Callback para procesar mensajes.
	//-void nombre(const ENetEvent& message).
	typedef OSKnet_message_callback OSKnet_message_received_callback;



	//Callback para procesar nuevas conexiones.
	//-void nombre(const ENetEvent& message).
	typedef OSKnet_message_callback OSKnet_new_connection_callback;

}