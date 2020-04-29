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

	//Bot�n (interfaz de usuario).
	class Button : public BaseUIElement, public IUserInterfaceImage, public IUserInterfaceText, public IUserInterfaceMouseInput {

	public:

		//Crea un bot�n en el �rea <rectangle>.
		Button(const Vector4& rectangle);


		//Destruye el bot�n.
		~Button();


		//Renderiza el bot�n.
		void Draw(const RenderAPI& renderer) override;

	protected:

	};

}
