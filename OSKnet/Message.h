#pragma once

#include "Types.h"

#include <string>
#include <stdexcept>

#include <enet/enet.h>

#include "Macros.h"

namespace OSK::NET {

	//representa un mensaje para la comunicaci�n srvidor-cliente y cliente-servidor.
	struct OSKAPI_CALL Message {

	public:

		//Crea un mensaje vac�o.
		Message();


		//Crea un mensaje, creado al ser recibido.
		Message(const ENetEvent& msg);


		//Destructor del mensaje.
		//Llama a Clear().
		~Message();


		//Mensaje de ENET.
		ENetEvent enetMessage{};


		//Obtiene la direcci�n del cliente que envi� el mensaje.
		//(S�lamente si el mensaje ha sido recibido por el servidor).
		ENetPeer* GetSenderConnection() const;


		//Obtiene la informaci�n del mensaje sin procesar.
		//(S�lamente si el mensaje ha sido recibido por el cliente/servidor).
		const char* GetRawData();


		//Obtiene el c�digo del mensaje.
		//(S�lamente si el mensaje ha sido recibido por el cliente/servidor).
		messageCode_t GetMessageCode();


		//Obtiene la informaci�n del mensaje sin procesar, ignorando el c�digo.
		//(S�lamente si el mensaje ha sido recibido por el cliente/servidor).
		char* GetData(const size_t& offset = 0, const size_t& size = 0);


		//Obtiene los pr�ximos <size> caracteres del mensaje desde la �ltima vez que se llam� a Read.
		char* Read(const size_t& size);


		//Establece el c�digo del mensaje.
		//(S�lamente si el mensaje va a ser enviado).
		//-code: c�digo del mensaje.
		void SetCode(const messageCode_t& code);


		//A�ade informaci�n al mensaje.
		//(S�lamente si el mensaje va a ser enviado).
		//-data: informaci�n a�adida al mensaje.
		void AddData(const char* data);


		//INTERNAL.
		//Obtiene la informaci�n que ser� enviada.
		//(S�lamente si el mensaje va a ser enviado).
		const char* GetRawDataToBeSent();


		//A�ade un string al mensaje.
		//(S�lamente si el mensaje va a ser enviado).
		//-data: informaci�n a�adida al mensaje.
		void Append(const std::string& data);


		//Limpia el buffer en el que se guarda la informaci�n en formato char*.
		void Clear();


	private:

		//Mensaje a enviar.
		std::string sendMessage = "";

		char* c_message = nullptr;

		size_t messageLength = 0;

		size_t cursor = 1;

		//C�digo del mensaje a enviar.
		messageCode_t code = 0;

	};

}