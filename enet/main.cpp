//#define OSK_DLL
#ifndef OSK_DLL

#include <enet/enet.h>

#include <iostream>

#include "OSKnet.hpp"
#include "Client.h"
#include "Server.h"
#include "Encoder.hpp"

#include "StructReg.h"
#include "Types.h"


struct packetTextMessage_t {
	const char* Message;
};

struct packetVectorMessage_t {
	int X;
	int Y;
	int Z;
};

//#define NO_NET

#ifndef NO_NET

#define SERVER

#ifndef SERVER
#define CLIENT
#endif

#endif

namespace OSK::NET {
	bool send_message = false;

	void newConnectionCallback(Message& msg) {
		std::cout << "NEW CONNECTION: " << msg.GetSenderConnection() << std::endl;
		send_message = true;
	}

	void messageCallback(Message& msg) {
		std::cout << "NEW MESSAGE" << std::endl;
		std::cout << "DATA = " << msg.GetRawData() << std::endl;
		std::cout << "ID = " << msg.GetMessageCode() << std::endl;	

		std::cout << "FLOATS: " << msg.CastFromMessageData<packetVectorMessage_t>(0, sizeof(packetVectorMessage_t)).X << std::endl;

		send_message = true;
	}

	int main() {

		bool run = true;

		OSK::NET::GlobalInit();

#ifdef NO_NET

		Message msg{};
		msg.SetCode(3);
		msg.AddData("XDXDXDXD");

		ENetEvent e{};
		ENetPacket* packet = new ENetPacket{};
		e.packet = packet;
		e.packet->data = (unsigned char*)msg.GetRawDataToBeSent();
		e.packet->dataLength = strlen(msg.GetRawDataToBeSent()) + 1;
		Message out{ e };

		std::cout << "ORIGINAL: " << msg.GetRawDataToBeSent() << std::endl;
		std::cout << "ORIGINAL-UCHAR*: " << (unsigned char*)msg.GetRawDataToBeSent() << std::endl;

		std::cout << "SENT: " << e.packet->data << std::endl;

		std::cout << "CODE = " << out.GetMessageCode() << std::endl;
		std::cout << "RAW DATA = " << out.GetRawData() << std::endl;
		std::cout << "STRING DATA = " << std::string(out.GetRawData()) << std::endl;
		std::cout << "NORMALIZED DATA = " << out.GetNormalizedData() << std::endl;

		delete packet;

#endif // NO_N


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

				Message msg{};
				msg.SetCode(0); 
				msg.AddData((char*)12);
				msg.AddData((char*)24);
				msg.AddData((char*)36);

				server.SendMessageToAll(msg);

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

				Message msg{};
				msg.SetCode(3);
				msg.AddData("XDXXDXDD");

				client.SendMessage(msg);
				
				client.Flush();

				send_message = false;
			}
		}

		client.Disconnect();
#endif
		OSK::NET::GlobalShutdown();

		return 0;
	}
}

int main() {
	return OSK::NET::main();
}



#endif // !OSK_DLL