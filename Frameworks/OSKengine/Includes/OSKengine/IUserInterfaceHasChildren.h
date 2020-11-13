#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "BaseUIElement.h"

namespace OSK::UI {

	OSK_INFO_INTERNAL
	//Interfaz para elementos de UI que tienen elementos hijos.
	class OSKAPI_CALL IUserInterfaceHasChildren {

	public:

		//Destructor de la interfaz.
		virtual ~IUserInterfaceHasChildren() = default;


		//Renderiza los elementos hijos.
		void Draw(SpriteBatch& spriteBatch);


		//Añade un nuevo elemento hijo.
		void AddElement(BaseUIElement* element);


		//Quita un elemento hijo.
		void RemoveElement(BaseUIElement* element);


		//Actualiza la posición de todos los elementos hijos.
		void UpdateChildrenPositions();


		//Elementos hijos.
		std::vector<BaseUIElement*> Children;
		
	protected:

		IUserInterfaceHasChildren(BaseUIElement* base);
		
		BaseUIElement* base;

	};

}
