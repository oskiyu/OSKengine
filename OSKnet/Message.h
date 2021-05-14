#pragma once

#include "Types.h"

#include <string>
#include <stdexcept>

#include <enet/enet.h>
#include <vector>
#include "Macros.h"

namespace OSK::NET {


	struct OSKAPI_CALL DataBuffer {

	public:

		void Write(byte_t* data, size_t size) {
			for (size_t i = 0; i < size; i++)
				bytes.push_back(data[i]);
		}

		byte_t* Read(size_t offset) {
			return &bytes.data()[offset];
		}

		template <typename T> T CastTo() const {
			return *reinterpret_cast<T*>(bytes.data());
		}

		template <typename T> T CastTo(size_t offset) const {
			return *reinterpret_cast<T*>(Read(offset));
		}

		size_t GetSize() const {
			return bytes.size();
		}

	private:

		std::vector<byte_t> bytes;

	};


	//representa un mensaje para la comunicación servidor-cliente y cliente-servidor.
	struct OSKAPI_CALL Message {

		friend class Client;
		friend class Server;

	public:

		//Crea un mensaje vacío.
		Message();


		//Crea un mensaje, creado al ser recibido.
		Message(const ENetEvent& msg);


		byte_t* GetData(size_t offset);

		template <typename T> T Read() {
			size_t oldCursor = cursor;
			cursor += sizeof(T);

			return buffer.CastTo<T>(oldCursor);
		}

		template <typename T> void Write(const T& value) {
			buffer.Write(reinterpret_cast(value), sizeof(T));
		}

		template <typename T> void WriteCopy(T value) {
			buffer.Write(reinterpret_cast(value), sizeof(T));
		}

		void WriteString(const std::string& data) {
			Write<stringLength_t>(data.length());
			buffer.Write((byte_t*)data.c_str(), data.length());
		}

		std::string ReadString() {
			int length = Read<stringLength_t>();
			std::string output = "";

			for (int i = 0; i < length; i++)
				output += Read<char>();

			return output;
		}

		//Mensaje de ENET.
		ENetEvent enetMessage{};

		//Obtiene la dirección del cliente que envió el mensaje.
		//(Sólamente si el mensaje ha sido recibido por el servidor).
		ENetPeer* GetSenderConnection() const;

	private:

		DataBuffer buffer;
		size_t cursor = 0;

	};

}