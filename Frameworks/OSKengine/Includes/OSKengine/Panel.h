#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "BaseUIElement.h"
#include "IUserInterfaceImage.h"
#include "IUserInterfaceHasChildren.h"

namespace OSK::UI {

	//Panel (interfaz de usuario).
	class OSKAPI_CALL Panel : public BaseUIElement, public IUserInterfaceImage, public IUserInterfaceHasChildren {

	public:

		//Crea un panel en el �rea <rectangle>.
		Panel(const Vector4& rectangle);


		//Destructor del panel.
		~Panel();

		//Renderiza el panel.
		void Draw(SpriteBatch& spriteBatch) override;


		//Actualiza su posici�n respecto al elemento <element>.
		void SetPositionRelativeTo(BaseUIElement* element) override;

	};

}
