#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "BaseUIElement.h"
#include "OldTexture.h"
#include "SpriteBatch.h"

namespace OSK::UI {

	OSK_INFO_INTERNAL
	//Interfaz para elementos de UI que tienen imagen.
	class OSKAPI_CALL IUserInterfaceImage {

	public:
		
		//Destructor de la interfaz.
		virtual ~IUserInterfaceImage() = default;


		//Renderiza la imagen.
		//DEPRECATED.
		void Draw(const RenderAPI& renderer, const BaseUIElement& base);


		//Renderiza la imagen.
		void Draw(SpriteBatch& spriteBatch, const BaseUIElement& base);


		void SetSprite(Sprite sprite);


		//Color de la imagen.
		Color ImageColor = Color(1.0f);


		//Textura a renderizar.
		//DEPRECATED.
		OldTexture* ImageTexture = nullptr;


		Sprite Image{};


		Vector4* Rectangle = nullptr;
		
	};

}