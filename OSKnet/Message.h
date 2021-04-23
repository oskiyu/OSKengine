#pragma once

#include "Types.h"

#include <string>
#include <stdexcept>

#include <enet/enet.h>

#include "Macros.h"

namespace OSK::NET {

	//representa un mensaje para la comunicación srvidor-cliente y cliente-servidor.
	struct OSKAPI_CALL Message {

	public:

		//Crea un mensaje vacío.
		Message();


		//Crea un mensaje, creado al ser recibido.
		Message(const ENetEvent& msg);


		//Destructor del mensaje.
		//Llama a Clear().
		~Message();


		//Mensaje de ENET.
		ENetEvent enetMessage{};


		//Obtiene la dirección del cliente que envió el mensaje.
		//(Sólamente si el mensaje ha sido recibido por el servidor).
		ENetPeer* GetSenderConnection() const;


		//Obtiene la información del mensaje sin procesar.
		//(Sólamente si el mensaje ha sido recibido por el cliente/servidor).
		const char* GetRawData();


		//Obtiene el código del mensaje.
		//(Sólamente si el mensaje ha sido recibido por el cliente/servidor).
		messageCode_t GetMessageCode();


		//Obtiene la información del mensaje sin procesar, ignorando el código.
		//(Sólamente si el mensaje ha sido recibido por el cliente/servidor).
		char* GetData(const size_t& offset = 0, const size_t& size = 0);


		//Obtiene los próximos <size> caracteres del mensaje desde la última vez que se llamó a Read.
		char* Read(const size_t& size);


		//Establece el código del mensaje.
		//(Sólamente si el mensaje va a ser enviado).
		//-code: código del mensaje.
		void SetCode(const messageCode_t& code);


		//Añade información al mensaje.
		//(Sólamente si el mensaje va a ser enviado).
		//-data: información añadida al mensaje.
		void AddData(const char* data);


		//INTERNAL.
		//Obtiene la información que será enviada.
		//(Sólamente si el mensaje va a ser enviado).
		const char* GetRawDataToBeSent();


		//Añade un string al mensaje.
		//(Sólamente si el mensaje va a ser enviado).
		//-data: información añadida al mensaje.
		void Append(const std::string& data);


		//Limpia el buffer en el que se guarda la información en formato char*.
		void Clear();


	private:

		//Mensaje a enviar.
		std::string sendMessage = "";

		char* c_message = nullptr;

		size_t messageLength = 0;

		size_t cursor = 1;

		//Código del mensaje a enviar.
		messageCode_t code = 0;

	};

}