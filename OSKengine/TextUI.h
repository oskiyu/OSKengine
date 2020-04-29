#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "BaseUIElement.h"
#include "IUserInterfaceText.h"

namespace OSK::UI {

	//Texto (interfaz de usuario).
	class Text : public BaseUIElement, public IUserInterfaceText {

	public:

		//Crea un texto en el área <rectangle> y con el texto <text> (por defecto = "").
		Text(const Vector4& rectangle, const std::string& text = "");


		//Destructor del texto.
		~Text();


		//Renderiza el texto.
		void Draw(const RenderAPI& renderer) override;

	};

}
