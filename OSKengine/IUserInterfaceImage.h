#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "BaseUIElement.h"
#include "Texture.h"

namespace OSK::UI {

	OSK_INFO_INTERNAL
	//Interfaz para elementos de UI que tienen imagen.
	class IUserInterfaceImage {

	public:
		
		//Destructor de la interfaz.
		virtual ~IUserInterfaceImage() = default;


		//Renderiza la imagen.
		void Draw(const RenderAPI& renderer, const BaseUIElement& base);


		//Color de la imagen.
		Color ImageColor = Color(1.0f);


		//Textura a renderizar.
		Texture* ImageTexture = nullptr;
		
	};

}