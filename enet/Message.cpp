#include "Message.h"

namespace OSK::NET {

	Message::Message() {

	}

	Message::Message(const ENetEvent& msg) {
		enetMessage = msg;
	}

	Message::~Message() {
		Clear();
	}

	ENetPeer* Message::GetSenderConnection() const {
		return enetMessage.peer;
	}

	const char* Message::GetRawData() {
		const char* buffer = reinterpret_cast<const char*>(enetMessage.packet->data);

		Clear();
		c_message = new char[enetMessage.packet->dataLength];
		strcpy(c_message, buffer);

		return c_message;
	}

	messageCode_t Message::GetMessageCode() {
		return GetRawData()[0];
	}

	char* Message::GetData(const size_t& offset, const size_t& size) {
		const char* originalData = GetRawData();

		char* data = new char[strlen(originalData) + 1];
		strcpy(data, originalData);

		Clear();

		c_message = new char[size + 1];
		strncpy(c_message, data + 1 + offset, size);
		c_message[size] = '\0';
		delete[] data;

		return c_message;
	}

	char* Message::Read(const size_t& size) {
		size_t oldCursor = cursor;
		cursor += size;

		Clear();
		c_message = GetData(oldCursor, size);

		return c_message;
	}

	void Message::SetCode(const messageCode_t& code) {
		this->code = code;
	}

	void Message::AddData(const char* data) {
		sendMessage.append(data);
	}

	const char* Message::GetRawDataToBeSent() {
		std::string buffer = std::to_string(code);
		buffer.append(sendMessage);

		Clear();
		c_message = new char[buffer.length() + 1];
		strcpy(c_message, buffer.c_str());

		return (const char*)c_message;
	}

	void Message::Append(const std::string& data) {
		sendMessage.append(data);
	}

	void Message::Clear() {
		if (c_message != nullptr) {
			delete[] c_message;
			c_message = nullptr;
		}
	}

}