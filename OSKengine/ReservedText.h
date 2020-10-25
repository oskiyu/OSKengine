#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "SpriteContainer.h"

namespace OSK {

	//Texto resrvado.
	//Reserva memoria para almacenar los sprites del texto.
	//Más eficienete que renderizar texto de otra manera.
	class OSKAPI_CALL ReservedText {

		friend class SpriteBatch;

	public:

		//Crea un texto reservado.
		ReservedText();

		//Elimina el texto reservado.
		~ReservedText();

		//Reserva memoria para un texto de <length> caracteres.
		void Reserve(const uint32_t& length);

		//Establece el texto.
		//Puede llamar a Reserve() si es necesario.
		void SetText(const std::string& text);

		//Libera memoria.
		void Clear();

	private:

		SpriteContainer spriteContainer;

		std::string text;

	};

}
