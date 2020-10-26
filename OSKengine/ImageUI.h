#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "BaseUIElement.h"
#include "IUserInterfaceImage.h"

namespace OSK::UI {

	//Una imagen (interfaz de usuario).
	class OSKAPI_CALL Image : public BaseUIElement, public IUserInterfaceImage {

	public:

		//Crea una imagen en el área <rectangle>.
		Image(const Vector4& rectangle);
		
		
		//Destruye la imagen.
		~Image();

		//Renderiza la imagen.
		void Draw(SpriteBatch& spriteBatch) override;
		
	private:

	};

}
