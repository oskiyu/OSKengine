#pragma once

#include <OSKnet/Client.h>

namespace OSKnet {

	public ref class Client {

	public:

		Client();

		~Client();

		!Client();

		void Setup(unsigned int channel_count, unsigned int in_bandwidht, unsigned int out_bandwidht);

		bool Connect(System::String^ ip, unsigned int port);

		void ProccessMessages();

		void SendMessage(Message& message, unsigned int channel);

		void Flush();

		void Disconnect();

	private:

		void wrapper_close();

		OSK::NET::Client* client = nullptr;

	};

}