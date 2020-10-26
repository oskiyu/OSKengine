#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include <string>

#include "BaseUIElement.h"
#include "ImageUI.h"
#include "TextUI.h"

#include "IUserInterfaceImage.h"
#include "IUserInterfaceText.h"
#include "IUserInterfaceMouseInput.h"

namespace OSK::UI {

	//Botón (interfaz de usuario).
	class OSKAPI_CALL Button : public BaseUIElement, public IUserInterfaceImage, public IUserInterfaceText, public IUserInterfaceMouseInput {

	public:

		//Crea un botón en el área <rectangle>.
		Button();


		//Destruye el botón.
		~Button();

		void SetPositionRelativeTo(BaseUIElement* element) override;

		//Renderiza el botón.
		void Draw(SpriteBatch& spriteBatch) override;

	protected:

	};

}
