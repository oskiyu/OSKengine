#pragma once

#include "Types.h"

#include <string>

#include <enet/enet.h>

namespace OSK::NET {

	//representa un mensaje para la comunicación srvidor-cliente y cliente-servidor.
	struct Message {

	public:

		//Crea un mensaje vacío.
		Message() {

		}


		//Crea un mensaje, creado al ser recibido.
		Message(const ENetEvent& msg) {
			enetMessage = msg;
		}


		//Destructor del mensaje.
		//Llama a Clear().
		~Message() {
			Clear();
		}


		//Mensaje de ENET.
		ENetEvent enetMessage{};


		//Obtiene la dirección del cliente que envió el mensaje.
		//(Sólamente si el mensaje ha sido recibido por el servidor).
		inline ENetPeer* GetSenderConnection() const {
			return enetMessage.peer;
		}


		//Obtiene la información del mensaje sin procesar.
		//(Sólamente si el mensaje ha sido recibido por el servidor).
		inline const char* GetRawData() {
			const char* buffer = reinterpret_cast<const char*>(enetMessage.packet->data);
			
			Clear();
			c_message = new char[enetMessage.packet->dataLength];
			strcpy(c_message, buffer);

			return c_message;
		}


		//Obtiene el código del mensaje.
		//(Sólamente si el mensaje ha sido recibido por el servidor).
		inline messageCode_t GetMessageCode() {
			return GetRawData()[0];
		}


		//Obtiene la información del mensaje sin procesar, ignorando el código.
		//(Sólamente si el mensaje ha sido recibido por el servidor).
		inline char* GetNormalizedData(const size_t& offset = 0, const size_t& size = 0) {
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


		//Procesa el mensaje.
		//-<T>: estructura de datos al que se va a convertir la información del bloque procesado.
		//-offset: offset desde el cual se comienza a procesar el mensaje.
		//-size: tamaño del bloque de información que hay que procesar.
		//(Sólamente si el mensaje ha sido recibido por el servidor).
		template <typename T> inline T CastFromMessageData(const size_t& offset = 0, const size_t& size = 0) {
			return *reinterpret_cast<T*>(GetNormalizedData(offset, size));
		}


		//Establece el código del mensaje.
		//(Sólamente si el mensaje va a ser enviado).
		//-code: código del mensaje.
		inline void SetCode(const messageCode_t& code) {
			this->code = code;
		}


		//Añade información al mensaje.
		//(Sólamente si el mensaje va a ser enviado).
		//-data: información añadida al mensaje.
		inline void AddData(const char* data) {
			sendMessage.append(data);
		}


		//INTERNAL.
		//Obtiene la información que será enviada.
		//(Sólamente si el mensaje va a ser enviado).
		inline const char* GetRawDataToBeSent() {
			std::string buffer = std::to_string(code);
			buffer.append(sendMessage);

			Clear();
			c_message = new char[buffer.length() + 1];
			strcpy(c_message, buffer.c_str());

			return (const char*)c_message;
		}


		//Limpia el buffer en el que se guarda la información en formato char*.
		inline void Clear() {
			if (c_message != nullptr) {
				delete[] c_message;
				c_message = nullptr;
			}
		}


	private:

		//Mensaje a enviar.
		std::string sendMessage = "";

		char* c_message = nullptr;

		//Código del mensaje a enviar.
		messageCode_t code = 0;

	};

}