#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "BaseUIElement.h"
#include "SpriteBatch.h"

namespace OSK::UI {

	OSK_INFO_INTERNAL
	//Interfaz para elementos de UI que tienen imagen.
	class OSKAPI_CALL IUserInterfaceImage {

	public:
		
		//Destructor de la interfaz.
		virtual ~IUserInterfaceImage() = default;



		//Renderiza la imagen.
		void Draw(SpriteBatch& spriteBatch, const BaseUIElement& base);


		void SetSprite(Sprite sprite);


		void UpdatePosition();


		//Color de la imagen.
		Color ImageColor = Color(1.0f);

		//Imagen a renderizar.
		Sprite ImageSprite{};

		//Posición base.
		Vector4* Rectangle = nullptr;
		
	};

}