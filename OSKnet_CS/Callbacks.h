#pragma once

//Callback general para procesar mensajes.
delegate void OSKnet_message_callback(Message^ message);

//Callback para procesar desconexiones.
//-void nombre(const ENetEvent& message).
delegate void OSKnet_disconnect_callback(Message^ message);

//Callback para procesar mensajes.
//-void nombre(const ENetEvent& message).
delegate void OSKnet_message_received_callback(Message^ message);

//Callback para procesar nuevas conexiones.
//-void nombre(const ENetEvent& message).
delegate void OSKnet_new_connection_callback(Message^ message);