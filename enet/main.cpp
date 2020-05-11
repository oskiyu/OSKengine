//#define OSK_DLL

#ifndef OSK_DLL

#include <enet/enet.h>

#include <iostream>

#include "Client.h"
#include "Server.h"
#include "Encoder.hpp"

#include "StructReg.h"
#include "Types.h"


struct packetTextMessage_t {
	const char* Message;
};

struct packetVectorMessage_t {
	float X;
	float Y;
	float Z;
};


//#define SERVER

#ifndef SERVER
#define CLIENT
#endif

namespace OSK::NET {
	bool send_message = false;

	void newConnectionCallback(const ENetEvent& msg) {
		std::cout << "NEW CONNECTION: " << msg.peer->host << std::endl;
		send_message = true;
	}

	void messageCallback(const ENetEvent& msg) {
		std::cout << "NEW MESSAGE" << std::endl;
		std::cout << "DATA = " << msg.packet->data << std::endl;
		std::cout << "ID= " << (uint32_t)msg.packet->data[0] << std::endl;		
		std::cout << "¿= " << *reinterpret_cast<float*>(msg.packet->data) << std::endl;

		send_message = true;
	}

	int main() {

		bool run = true;

		enet_initialize();
		atexit(enet_deinitialize);

#ifdef SERVER

		Server server = Server();
		server.Setup(SERVER_IP_ANY, 12345);

		server.SetNewConnectionCallback(newConnectionCallback);
		server.SetMessageReceivedCallback(messageCallback);

		if (!server.Start(5))
			std::cout << "ERROR: start server" << std::endl;

		int i = 0;
		while (run) {
			server.ProccessMessages();

			if (send_message) {
				packetTextMessage_t p = { "XD" };

				Packet xd = {};
				OSKnet_CREATE_PACKET(xd, OSKnet_GET_STRUCT_ID(packetTextMessage_t), &p);

				server.SendMessageToAll("XD");

				server.Flush();
				send_message = false;
			}
		}

		server.Close();

#endif

#ifdef CLIENT

		Client client = Client();
		client.Setup();

		if (!client.Connect("localhost", 12345))
			std::cout << "ERROR: start client" << std::endl;

		client.SetNewConnectionCallback(newConnectionCallback);
		client.SetMessageReceivedCallback(messageCallback);

		while (run) {
			client.ProccessMessages();
			if (send_message) {
				std::string msg = "0";
				packetTextMessage_t a{ "XDXD" };
				msg.append(reinterpret_cast<char*>(&a));
				std::cout << msg << std::endl;
				client.SendMessage(msg.c_str());
				client.Flush();

				send_message = false;
			}
		}

		client.Disconnect();
#endif

		return 0;
	}
}

int main() {
	return OSK::NET::main();
}



#endif // !OSK_DLL