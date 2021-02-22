#include "Macros.h"
#define OSK_DLL
#ifndef OSK_DLL

#include <enet/enet.h>

#include <iostream>

#include "OSKnet.hpp"
#include "Client.h"
#include "Server.h"
#include "Encoder.hpp"

#include "StructReg.h"
#include "Types.h"

//#define NO_NET

#ifndef NO_NET

//#define SERVER

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

		int num = 0;
		num = *reinterpret_cast<int*>(msg.GetData(0, sizeof(int)));
		std::cout << num << std::endl;
		num = *reinterpret_cast<int*>(msg.GetData(sizeof(int), sizeof(int)));
		std::cout << num << std::endl;
		num = *reinterpret_cast<int*>(msg.GetData(sizeof(int) * 2, sizeof(int)));
		std::cout << num << std::endl;

		send_message = true;
	}

	int main() {

		bool run = true;

		OSK::NET::GlobalInit();

#ifdef NO_NET

		Message msg{};
		msg.SetCode(3);

		packetVectorMessage_t p;
		p.X = 3;
		p.Y = 4;
		p.Z = 5;

		msg.AddData(p.X);
		msg.AddData(p.Y);
		msg.AddData(p.Z);

		const char* data = reinterpret_cast<const char*>(&p);
		std::cout << "DATA_T" << data << std::endl;
		std::cout << "DATA[0]" << data[0] << std::endl;
		std::cout << "DATA[1]" << data[sizeof(int)] << std::endl;
		std::cout << "DATA[2]" << data[sizeof(int) * 2] << std::endl;

		ENetEvent e{};
		ENetPacket* packet = new ENetPacket{};
		e.packet = packet;
		e.packet->data = (unsigned char*)msg.GetRawDataToBeSent();
		e.packet->dataLength = strlen(msg.GetRawDataToBeSent()) + 1;
		Message out{ e };

		std::cout << "ORIGINAL-UCHAR*: " << (unsigned char*)msg.GetRawDataToBeSent() << std::endl;
		std::cout << "ORIGINAL-DATA-LENGTH: " << e.packet->dataLength << "/" << strlen(msg.GetRawDataToBeSent()) + 1 << std::endl;

		std::cout << "SENT: " << e.packet->data << std::endl;

		std::cout << "CODE = " << out.GetMessageCode() << std::endl;
		std::cout << "RAW DATA = " << out.GetRawData() << std::endl;
		std::cout << "STRING DATA = " << std::string(out.GetRawData()) << std::endl;
		std::cout << "NORMALIZED DATA = " << out.GetNormalizedData() << std::endl;

		char* ndata = out.GetNormalizedData(0, sizeof(packetVectorMessage_t));
		packetVectorMessage_t xd = *reinterpret_cast<packetVectorMessage_t*>(ndata);
		std::cout << "VECTOR DATA = " << xd.X << std::endl;
		
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

				msg.Append(std::to_string(4));
				msg.Append(std::to_string(5));
				msg.Append(std::to_string(6));

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
				msg.SetCode(0);

				msg.AddData("7");
				msg.AddData("8");
				msg.AddData("9");

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