#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include <functional>
#include <vector>

#include "AnchorEnum.h"
#include "UIAnchorTypeEnum.h"
#include "RenderAPI.h"
#include "OldTexture.h"
#include "UIInterfacesBitFlags.h"
#include "SpriteBatch.h"

namespace OSK::UI {
	
	OSK_INFO_DO_NOT_TOUCH
	//Clase base para cualquier elemento de interfaz de usuario.
	class OSKAPI_CALL BaseUIElement {

	public:

		//Destruye el elemento de UI.
		virtual ~BaseUIElement() = default;


		//Dibuja el elemento de UI.
		//DEPRECATED.
		virtual void Draw(const RenderAPI& renderer) = 0;


		//Dibuja el elemento de UI.
		virtual void Draw(SpriteBatch& spriteBatch) = 0;


		//Actualiza la posición de este elemento dentro de los límites del elemento <element>.
		virtual void SetPositionRelativeTo(BaseUIElement* element);


		//Establece el área del elemento.
		void SetRectangle(const Vector4& rectangle);


		//Área de la pantalla en la que se euncuentra este elemento.
		Vector4 GetRectangle() const;

		
		//Elemento de UI padre.
		BaseUIElement* Parent = nullptr;


		//Anchor del elemento respecto al elemento padre / pantalla.
		Anchor ElementAnchor = Anchor::BOTTOM_LEFT;


		//Tipo de anclaje del elemento:
		//  -UIAnchorType::PARENT: elemento padre.
		//  -UIAnchorType::WINDOW: pantalla.
		UIAnchorType AnchorType = UIAnchorType::WINDOW;

		
		//Opacidad del elemento (no se aplica a elementos hijos).
		float_t Opacity = 1.0f;


		//Determina si este elemento se va a renderizar.
		bool IsVisible = true;


		//Determina si este elemento está activo.
		bool IsActive = true;


		//Determina si este elemento debe expandirse para ocupar todo el elemento padre / pantalla.
		bool FillParent = false;

	protected:

		BaseUIElement(const Vector4& rectangle);

		Vector4 originalRectangle = Vector4(0.0f);

		Vector4 rectangle = Vector4(0.0f);

	};

}
