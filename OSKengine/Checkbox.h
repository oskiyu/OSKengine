#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "BaseUIElement.h"
#include "IUserInterfaceImage.h"
#include "IUserInterfaceMouseInput.h"

namespace OSK::UI {

	//Checkbox (interfaz de usuario).
	//Representa un 'bot�n' sin texto que puede ser activado o desactivado.
	class OSKAPI_CALL Checkbox : public BaseUIElement, public IUserInterfaceImage, public IUserInterfaceMouseInput {

	public:

		//Crea un checkbox en el �rea <rectangle>.
		Checkbox(const Vector4& rectangle);


		//Destruye el checkbox.
		~Checkbox();


		//Renderiza el checkbox.
		void Draw(const RenderAPI& renderer) override;


		//Renderiza el checkbox.
		void Draw(SpriteBatch& spriteBatch) override;


		//Puntero al bool que representar� el estado del checkbox.
		//Su valor se actualizar� autom�ticamente al presionarse el checkbox.
		bool* LinkedValue = nullptr;

	private:
		
		void leftClick() override;
			   
	};

}