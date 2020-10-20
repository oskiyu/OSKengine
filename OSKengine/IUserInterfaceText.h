#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include <string>

#include "BaseUIElement.h"
#include "AnchorEnum.h"
#include "OldFont.h"
#include "SpriteBatch.h"
#include "Font.h"

namespace OSK::UI {

	OSK_INFO_INTERNAL
	//Interfaz para elementos de UI que tienen texto.
	class OSKAPI_CALL IUserInterfaceText {

	public:

		//Destructor de la interfaz.
		virtual ~IUserInterfaceText() = default;


		//Renderiza el texto.
		//DEPRECATED.
	//	void Draw(const RenderAPI& renderer, const BaseUIElement& base);


		//Renderiza el texto.
		void Draw(SpriteBatch& spriteBatch, const BaseUIElement& base);


		//Texto a renderizar, si <LinkedText> == nullptr.
		std::string Texto = "";


		//Enlace al texto que se va a renderizar.
		//Cualquier cambio al string original se verá reflejado en este elemento de UI.
		std::string* LinkedText = nullptr;


		//Punto de anclaje del texto.
		Anchor TextAnchor = Anchor::CENTER;


		//Fuente del texto.
		OldFont* OldTextFont = nullptr;


		//Fuente del texto.
		Font* TextFont = nullptr;


		//Tamaño del texto.
		float_t TextSize = 1;


		//Color del texto.
		Color TextColor = Color(1.0f);


		//Espacio entre el texto y el rectángulo el elemento de UI.
		float_t TextBorderLimit = 5.0f;

	};
	
}
