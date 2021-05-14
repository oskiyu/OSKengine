#include "Message.h"

namespace OSK::NET {

	Message::Message() {

	}

	Message::Message(const ENetEvent& msg) {
		enetMessage = msg;

		buffer.Write(enetMessage.packet->data, enetMessage.packet->dataLength);
	}

	ENetPeer* Message::GetSenderConnection() const {
		return enetMessage.peer;
	}

	byte_t* Message::GetData(size_t offset) {
		return buffer.Read(offset);
	}

}
